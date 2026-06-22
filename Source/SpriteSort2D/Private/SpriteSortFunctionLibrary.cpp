#include "SpriteSortFunctionLibrary.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"

float USpriteSortFunctionLibrary::CalculateSortValueFromLocation(FVector WorldLocation, FVector SortAxis, bool bInvertSort)
{
	const FVector NormalizedSortAxis = SortAxis.GetSafeNormal();
	if (NormalizedSortAxis.IsNearlyZero())
	{
		return 0.f;
	}

	float SortValue = FVector::DotProduct(WorldLocation, NormalizedSortAxis);
	if (bInvertSort)
	{
		SortValue *= -1.f;
	}

	return SortValue;
}

FVector USpriteSortFunctionLibrary::CalculateDepthOffset(float SortValue, FVector CameraDepthAxis, float DepthScale)
{
	const FVector NormalizedDepthAxis = CameraDepthAxis.GetSafeNormal();
	if (NormalizedDepthAxis.IsNearlyZero())
	{
		return FVector::ZeroVector;
	}

	return NormalizedDepthAxis * SortValue * DepthScale;
}

FVector USpriteSortFunctionLibrary::CalculateBoundsBaseLocation(FBoxSphereBounds Bounds, FVector BoundsBaseAxis, FVector SortOffset)
{
	const FVector NormalizedBaseAxis = BoundsBaseAxis.GetSafeNormal();
	if (NormalizedBaseAxis.IsNearlyZero())
	{
		return Bounds.Origin + SortOffset;
	}

	const FVector Extent = Bounds.BoxExtent;
	const float ProjectedExtent =
		FMath::Abs(NormalizedBaseAxis.X) * Extent.X
		+ FMath::Abs(NormalizedBaseAxis.Y) * Extent.Y
		+ FMath::Abs(NormalizedBaseAxis.Z) * Extent.Z;

	return Bounds.Origin - NormalizedBaseAxis * ProjectedExtent + SortOffset;
}

void USpriteSortFunctionLibrary::ApplyVisualDepthOffset(USceneComponent* VisualRoot, FTransform OriginalRelativeTransform, FVector DepthOffset)
{
	if (!IsValid(VisualRoot))
	{
		return;
	}

	FTransform NewTransform = OriginalRelativeTransform;
	NewTransform.SetLocation(OriginalRelativeTransform.GetLocation() + DepthOffset);
	VisualRoot->SetRelativeTransform(NewTransform);
}

USceneComponent* USpriteSortFunctionLibrary::FindBestPaper2DVisualComponent(AActor* Owner)
{
	if (!IsValid(Owner))
	{
		return nullptr;
	}

	TArray<USceneComponent*> SceneComponents;
	Owner->GetComponents(SceneComponents);

	for (USceneComponent* SceneComponent : SceneComponents)
	{
		if (IsValid(SceneComponent) && SceneComponent->GetName().Equals(TEXT("VisualRoot"), ESearchCase::IgnoreCase))
		{
			return SceneComponent;
		}
	}

	if (UPaperFlipbookComponent* Flipbook = Owner->FindComponentByClass<UPaperFlipbookComponent>())
	{
		return Flipbook;
	}

	if (UPaperSpriteComponent* Sprite = Owner->FindComponentByClass<UPaperSpriteComponent>())
	{
		return Sprite;
	}

	const USceneComponent* Root = Owner->GetRootComponent();
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents);

	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (!IsValid(Primitive) || Primitive == Root || IsLikelyCollisionComponent(Primitive))
		{
			continue;
		}

		return Primitive;
	}

	return nullptr;
}

UPrimitiveComponent* USpriteSortFunctionLibrary::FindBestPrimitiveTarget(AActor* Owner, USceneComponent* VisualRoot)
{
	if (!IsValid(Owner))
	{
		return nullptr;
	}

	if (UPaperFlipbookComponent* Flipbook = Owner->FindComponentByClass<UPaperFlipbookComponent>())
	{
		return Flipbook;
	}

	if (UPaperSpriteComponent* Sprite = Owner->FindComponentByClass<UPaperSpriteComponent>())
	{
		return Sprite;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents);

	if (IsValid(VisualRoot))
	{
		for (UPrimitiveComponent* Primitive : PrimitiveComponents)
		{
			if (IsValid(Primitive) && Primitive != VisualRoot && Primitive->IsAttachedTo(VisualRoot) && !IsLikelyCollisionComponent(Primitive))
			{
				return Primitive;
			}
		}

		if (UPrimitiveComponent* VisualPrimitive = Cast<UPrimitiveComponent>(VisualRoot))
		{
			return VisualPrimitive;
		}
	}

	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (IsValid(Primitive) && !IsLikelyCollisionComponent(Primitive))
		{
			return Primitive;
		}
	}

	return nullptr;
}

bool USpriteSortFunctionLibrary::IsLikelyCollisionComponent(const UPrimitiveComponent* Primitive)
{
	if (!IsValid(Primitive))
	{
		return false;
	}

	if (Primitive->IsA<UShapeComponent>())
	{
		return true;
	}

	const FString Name = Primitive->GetName();
	return Name.Contains(TEXT("Collision"), ESearchCase::IgnoreCase)
		|| Name.Contains(TEXT("Collider"), ESearchCase::IgnoreCase)
		|| Name.Contains(TEXT("Hitbox"), ESearchCase::IgnoreCase)
		|| Name.Contains(TEXT("Hurtbox"), ESearchCase::IgnoreCase)
		|| Name.Contains(TEXT("Capsule"), ESearchCase::IgnoreCase);
}
