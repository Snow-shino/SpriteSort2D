#include "SpriteSortComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "SpriteSortFunctionLibrary.h"
#include "SpriteSortSettings.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpriteSort2D, Log, All);

namespace
{
	constexpr float DefaultDrawDuration = 0.f;
}

USpriteSortComponent::USpriteSortComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	const USpriteSortSettings* Settings = GetDefault<USpriteSortSettings>();
	if (Settings)
	{
		SortAxis = Settings->DefaultSortAxis;
		CameraDepthAxis = Settings->DefaultCameraDepthAxis;
		BoundsBaseAxis = Settings->DefaultBoundsBaseAxis;
		DepthScale = Settings->DefaultDepthScale;
		MovementThreshold = Settings->DefaultMovementThreshold;
		WhenMovedTickInterval = Settings->DefaultWhenMovedTickInterval;
		UpdateMode = Settings->DefaultUpdateMode;
		OriginMode = Settings->DefaultOriginMode;
		bDebugDraw = Settings->bDebugDrawByDefault;
		bAutoFindVisualRoot = Settings->bAutoFindVisualRoot;
		bAutoFindTargetPrimitive = Settings->bAutoFindTargetPrimitive;
	}
}

void USpriteSortComponent::OnRegister()
{
	Super::OnRegister();
	UpdateTickEnabled();
}

void USpriteSortComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!VisualRoot && bAutoFindVisualRoot)
	{
		AutoFindVisualRoot();
	}

	if (!TargetPrimitive && bAutoFindTargetPrimitive)
	{
		AutoFindTargetPrimitive();
	}

	WarnIfVisualRootContainsCollision();
	CacheOriginalVisualTransform(true);
	ResetMovementBaseline();

	if (UpdateMode != ESpriteSortUpdateMode::Manual)
	{
		UpdateSortNow();
	}

	UpdateTickEnabled();
}

void USpriteSortComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestoreOriginalVisualTransform();
	Super::EndPlay(EndPlayReason);
}

void USpriteSortComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableSorting || UpdateMode == ESpriteSortUpdateMode::Manual || UpdateMode == ESpriteSortUpdateMode::OnBeginPlayOnly)
	{
		return;
	}

	if (UpdateMode == ESpriteSortUpdateMode::EveryTick || HasSortMovementChanged())
	{
		UpdateSortNow();
	}
}

void USpriteSortComponent::UpdateSortNow()
{
	if (!bEnableSorting)
	{
		RestoreOriginalVisualTransform();
		return;
	}

	if (!VisualRoot && bAutoFindVisualRoot)
	{
		AutoFindVisualRoot();
	}

	if (!TargetPrimitive && bAutoFindTargetPrimitive)
	{
		AutoFindTargetPrimitive();
	}

	if (!ValidateForSorting())
	{
		return;
	}

	CacheOriginalVisualTransform();

	const FVector SortWorldLocation = GetStableSortWorldLocation();
	CurrentSortValue = USpriteSortFunctionLibrary::CalculateSortValueFromLocation(SortWorldLocation, SortAxis, bInvertSort);
	CurrentVisualDepthOffset = USpriteSortFunctionLibrary::CalculateDepthOffset(CurrentSortValue, CameraDepthAxis, DepthScale);

	switch (SortingMode)
	{
	case ESpriteSortMode::VisualDepthOffset:
		ApplyVisualDepthOffset();
		break;
	case ESpriteSortMode::TranslucentPriorityFallback:
		RestoreOriginalVisualTransform();
		ApplyTranslucentPriorityFallback();
		break;
	case ESpriteSortMode::Manual:
	default:
		RestoreOriginalVisualTransform();
		CurrentVisualDepthOffset = FVector::ZeroVector;
		break;
	}

	if (bDebugDraw)
	{
		DrawDebugInfo();
	}

	SetMovementBaseline(SortWorldLocation);
}

void USpriteSortComponent::SetSortingEnabled(bool bEnabled)
{
	bEnableSorting = bEnabled;
	if (!bEnableSorting)
	{
		RestoreOriginalVisualTransform();
	}
	else
	{
		UpdateSortNow();
	}

	UpdateTickEnabled();
}

void USpriteSortComponent::SetVisualRoot(USceneComponent* NewVisualRoot)
{
	if (VisualRoot != NewVisualRoot)
	{
		RestoreOriginalVisualTransform();
		VisualRoot = NewVisualRoot;
		bHasOriginalRelativeTransform = false;
		CacheOriginalVisualTransform(true);
		WarnIfVisualRootContainsCollision();
		UpdateSortNow();
	}
}

void USpriteSortComponent::SetSortOrigin(USceneComponent* NewSortOrigin)
{
	SortOrigin = NewSortOrigin;
	ResetMovementBaseline();
	UpdateSortNow();
}

void USpriteSortComponent::SetSortOffset(FVector NewOffset)
{
	SortOffset = NewOffset;
	ResetMovementBaseline();
	UpdateSortNow();
}

void USpriteSortComponent::SetOriginMode(ESpriteSortOriginMode NewOriginMode)
{
	OriginMode = NewOriginMode;
	ResetMovementBaseline();
	UpdateSortNow();
}

void USpriteSortComponent::SetUpdateMode(ESpriteSortUpdateMode NewUpdateMode)
{
	UpdateMode = NewUpdateMode;
	UpdateTickEnabled();
	ResetMovementBaseline();

	if (UpdateMode != ESpriteSortUpdateMode::Manual)
	{
		UpdateSortNow();
	}
}

float USpriteSortComponent::GetCurrentSortValue() const
{
	return CurrentSortValue;
}

FVector USpriteSortComponent::GetSortWorldLocation() const
{
	FBoxSphereBounds Bounds;

	switch (OriginMode)
	{
	case ESpriteSortOriginMode::ExplicitSortOrigin:
		return IsValid(SortOrigin) ? SortOrigin->GetComponentLocation() : GetActorLocationPlusOffset();

	case ESpriteSortOriginMode::ActorLocationPlusOffset:
		return GetActorLocationPlusOffset();

	case ESpriteSortOriginMode::VisualBoundsBase:
		return TryGetVisualBounds(Bounds)
			? USpriteSortFunctionLibrary::CalculateBoundsBaseLocation(Bounds, BoundsBaseAxis, SortOffset)
			: GetActorLocationPlusOffset();

	case ESpriteSortOriginMode::TargetBoundsBase:
		return TryGetTargetBounds(Bounds)
			? USpriteSortFunctionLibrary::CalculateBoundsBaseLocation(Bounds, BoundsBaseAxis, SortOffset)
			: GetActorLocationPlusOffset();

	case ESpriteSortOriginMode::Auto:
	default:
		if (IsValid(SortOrigin))
		{
			return SortOrigin->GetComponentLocation();
		}

		if (TryGetVisualBounds(Bounds) || TryGetTargetBounds(Bounds))
		{
			return USpriteSortFunctionLibrary::CalculateBoundsBaseLocation(Bounds, BoundsBaseAxis, SortOffset);
		}

		return GetActorLocationPlusOffset();
	}
}

FVector USpriteSortComponent::GetCurrentVisualDepthOffset() const
{
	return CurrentVisualDepthOffset;
}

void USpriteSortComponent::RestoreOriginalVisualTransform()
{
	if (IsValid(VisualRoot) && bHasOriginalRelativeTransform)
	{
		VisualRoot->SetRelativeTransform(OriginalRelativeTransform);
	}

	CurrentVisualDepthOffset = FVector::ZeroVector;
}

void USpriteSortComponent::AutoFindVisualRoot()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	VisualRoot = USpriteSortFunctionLibrary::FindBestPaper2DVisualComponent(Owner);
	if (!VisualRoot)
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SpriteSortComponent could not find a VisualRoot. Add a scene component named VisualRoot or assign the visual component explicitly."), *Owner->GetName());
	}
	else
	{
		bHasOriginalRelativeTransform = false;
		CacheOriginalVisualTransform(true);
	}
}

void USpriteSortComponent::AutoFindTargetPrimitive()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	TargetPrimitive = USpriteSortFunctionLibrary::FindBestPrimitiveTarget(Owner, VisualRoot);
	if (!TargetPrimitive)
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SpriteSortComponent could not find a TargetPrimitive. Assign a Paper2D or primitive visual component if translucent fallback is needed."), *Owner->GetName());
	}
}

void USpriteSortComponent::CacheOriginalVisualTransform(bool bForce)
{
	if (!IsValid(VisualRoot))
	{
		return;
	}

	if (bForce || !bHasOriginalRelativeTransform)
	{
		OriginalRelativeTransform = VisualRoot->GetRelativeTransform();
		bHasOriginalRelativeTransform = true;
	}
}

void USpriteSortComponent::ResetMovementBaseline()
{
	SetMovementBaseline(GetStableSortWorldLocation());
}

void USpriteSortComponent::SetMovementBaseline(FVector NewSortWorldLocation)
{
	const AActor* Owner = GetOwner();
	LastOwnerLocation = IsValid(Owner) ? Owner->GetActorLocation() : FVector::ZeroVector;
	LastSortWorldLocation = NewSortWorldLocation;
	bHasMovementBaseline = true;
}

bool USpriteSortComponent::HasSortMovementChanged()
{
	if (!bHasMovementBaseline)
	{
		return true;
	}

	const AActor* Owner = GetOwner();
	const FVector OwnerLocation = IsValid(Owner) ? Owner->GetActorLocation() : FVector::ZeroVector;
	const FVector SortWorldLocation = GetStableSortWorldLocation();
	const float ThresholdSq = FMath::Square(FMath::Max(0.f, MovementThreshold));
	return FVector::DistSquared(OwnerLocation, LastOwnerLocation) > ThresholdSq
		|| FVector::DistSquared(SortWorldLocation, LastSortWorldLocation) > ThresholdSq;
}

bool USpriteSortComponent::ValidateForSorting() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}

	if (!IsValid(VisualRoot))
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SpriteSortComponent needs a VisualRoot before it can sort."), *Owner->GetName());
		return false;
	}

	if (SortAxis.IsNearlyZero())
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SpriteSortComponent SortAxis is zero. Use a non-zero world axis, usually Y for top-down Paper2D."), *Owner->GetName());
		return false;
	}

	if (CameraDepthAxis.IsNearlyZero())
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SpriteSortComponent CameraDepthAxis is zero. Use the world axis that should receive visual depth offsets."), *Owner->GetName());
		return false;
	}

	if ((OriginMode == ESpriteSortOriginMode::Auto || OriginMode == ESpriteSortOriginMode::VisualBoundsBase || OriginMode == ESpriteSortOriginMode::TargetBoundsBase) && BoundsBaseAxis.IsNearlyZero())
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SpriteSortComponent BoundsBaseAxis is zero. Use the axis that points from the sprite base toward the sprite top, usually Z."), *Owner->GetName());
		return false;
	}

	if (VisualRoot == Owner->GetRootComponent())
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: VisualRoot is the actor root. Sorting will visually move the root component; use a separate child VisualRoot to keep gameplay transform stable."), *Owner->GetName());
	}

	if (IsValid(SortOrigin) && SortOrigin->IsAttachedTo(VisualRoot))
	{
		UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: SortOrigin '%s' is attached under VisualRoot '%s'. Put the sort origin outside VisualRoot so the origin does not move with the visual depth offset."), *Owner->GetName(), *SortOrigin->GetName(), *VisualRoot->GetName());
	}

	return true;
}

void USpriteSortComponent::ApplyVisualDepthOffset()
{
	if (!IsValid(VisualRoot) || !bHasOriginalRelativeTransform)
	{
		return;
	}

	USpriteSortFunctionLibrary::ApplyVisualDepthOffset(VisualRoot, OriginalRelativeTransform, CurrentVisualDepthOffset);
}

void USpriteSortComponent::ApplyTranslucentPriorityFallback()
{
	if (!IsValid(TargetPrimitive))
	{
		return;
	}

	const int32 Priority = FMath::RoundToInt(CurrentSortValue * DepthScale);
	TargetPrimitive->SetTranslucentSortPriority(Priority);
}

void USpriteSortComponent::DrawDebugInfo()
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(VisualRoot))
	{
		return;
	}

	const FVector SortLocation = GetStableSortWorldLocation();
	const USceneComponent* Parent = VisualRoot->GetAttachParent();
	const FVector OriginalVisualLocation = Parent
		? Parent->GetComponentTransform().TransformPosition(OriginalRelativeTransform.GetLocation())
		: OriginalRelativeTransform.GetLocation();
	const FVector AdjustedVisualLocation = VisualRoot->GetComponentLocation();
	const FString VisualName = VisualRoot ? VisualRoot->GetName() : TEXT("None");
	const FString UpdateModeName = StaticEnum<ESpriteSortUpdateMode>()->GetNameStringByValue(static_cast<int64>(UpdateMode));
	const FString Label = FString::Printf(TEXT("Sort %.2f | Offset %s | %s | %s"), CurrentSortValue, *CurrentVisualDepthOffset.ToCompactString(), *UpdateModeName, *VisualName);

	DrawDebugSphere(World, SortLocation, 8.f, 12, FColor::Cyan, false, DefaultDrawDuration);
	DrawDebugCrosshairs(World, SortLocation, FRotator::ZeroRotator, 14.f, FColor::Cyan, false, DefaultDrawDuration);
	DrawDebugLine(World, OriginalVisualLocation, AdjustedVisualLocation, FColor::Magenta, false, DefaultDrawDuration, 0, 1.5f);
	DrawDebugString(World, SortLocation + FVector(0.f, 0.f, 24.f), Label, nullptr, FColor::White, DefaultDrawDuration, false);
}

void USpriteSortComponent::WarnIfVisualRootContainsCollision() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !IsValid(VisualRoot))
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents);

	if (const UPrimitiveComponent* VisualPrimitive = Cast<UPrimitiveComponent>(VisualRoot))
	{
		if (VisualPrimitive->IsA<UShapeComponent>() || USpriteSortFunctionLibrary::IsLikelyCollisionComponent(VisualPrimitive))
		{
			UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: VisualRoot '%s' looks like collision. Use a separate child VisualRoot that contains visuals only."), *Owner->GetName(), *VisualRoot->GetName());
		}
	}

	for (const UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (!IsValid(Primitive) || Primitive == VisualRoot)
		{
			continue;
		}

		if (Primitive->IsAttachedTo(VisualRoot) && (Primitive->IsA<UShapeComponent>() || USpriteSortFunctionLibrary::IsLikelyCollisionComponent(Primitive)))
		{
			UE_LOG(LogSpriteSort2D, Warning, TEXT("%s: VisualRoot '%s' appears to contain collision component '%s'. Put collision outside VisualRoot so sorting moves visuals only."), *Owner->GetName(), *VisualRoot->GetName(), *Primitive->GetName());
		}
	}
}

void USpriteSortComponent::UpdateTickEnabled()
{
	const bool bShouldTick = bEnableSorting
		&& (UpdateMode == ESpriteSortUpdateMode::WhenMoved || UpdateMode == ESpriteSortUpdateMode::EveryTick);

	PrimaryComponentTick.TickInterval = UpdateMode == ESpriteSortUpdateMode::WhenMoved
		? FMath::Max(0.f, WhenMovedTickInterval)
		: 0.f;

	SetComponentTickEnabled(bShouldTick);
}

bool USpriteSortComponent::UsesBoundsBasedOrigin() const
{
	if (OriginMode == ESpriteSortOriginMode::VisualBoundsBase || OriginMode == ESpriteSortOriginMode::TargetBoundsBase)
	{
		return true;
	}

	return OriginMode == ESpriteSortOriginMode::Auto && !IsValid(SortOrigin);
}

FVector USpriteSortComponent::GetStableSortWorldLocation()
{
	if (!UsesBoundsBasedOrigin() || !IsValid(VisualRoot) || !bHasOriginalRelativeTransform)
	{
		return GetSortWorldLocation();
	}

	const FTransform SavedRelativeTransform = VisualRoot->GetRelativeTransform();
	const bool bWasOffset = !SavedRelativeTransform.Equals(OriginalRelativeTransform);
	if (bWasOffset)
	{
		VisualRoot->SetRelativeTransform(OriginalRelativeTransform);
	}

	const FVector SortWorldLocation = GetSortWorldLocation();

	if (bWasOffset)
	{
		VisualRoot->SetRelativeTransform(SavedRelativeTransform);
	}

	return SortWorldLocation;
}

bool USpriteSortComponent::TryGetVisualBounds(FBoxSphereBounds& OutBounds) const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !IsValid(VisualRoot))
	{
		return false;
	}

	FBox BoundsBox(ForceInit);
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents);

	for (const UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (!IsValid(Primitive) || USpriteSortFunctionLibrary::IsLikelyCollisionComponent(Primitive))
		{
			continue;
		}

		if (Primitive == VisualRoot || Primitive->IsAttachedTo(VisualRoot))
		{
			BoundsBox += Primitive->Bounds.GetBox();
		}
	}

	if (!BoundsBox.IsValid)
	{
		if (const UPrimitiveComponent* VisualPrimitive = Cast<UPrimitiveComponent>(VisualRoot))
		{
			BoundsBox += VisualPrimitive->Bounds.GetBox();
		}
	}

	if (!BoundsBox.IsValid)
	{
		return false;
	}

	OutBounds = FBoxSphereBounds(BoundsBox);
	return true;
}

bool USpriteSortComponent::TryGetTargetBounds(FBoxSphereBounds& OutBounds) const
{
	if (!IsValid(TargetPrimitive))
	{
		return false;
	}

	OutBounds = TargetPrimitive->Bounds;
	return true;
}

FVector USpriteSortComponent::GetActorLocationPlusOffset() const
{
	const AActor* Owner = GetOwner();
	return IsValid(Owner) ? Owner->GetActorLocation() + SortOffset : SortOffset;
}

#if WITH_EDITOR
void USpriteSortComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateTickEnabled();
}
#endif
