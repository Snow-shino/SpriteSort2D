#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpriteSortTypes.h"
#include "SpriteSortComponent.generated.h"

class UPrimitiveComponent;
class USceneComponent;

UCLASS(ClassGroup = (SpriteSort2D), meta = (BlueprintSpawnableComponent, DisplayName = "Sprite Sort Component"))
class SPRITESORT2D_API USpriteSortComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpriteSortComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Advanced", meta = (AdvancedDisplay))
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Advanced", meta = (AdvancedDisplay))
	TObjectPtr<UPrimitiveComponent> TargetPrimitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Advanced", meta = (AdvancedDisplay))
	TObjectPtr<USceneComponent> SortOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ToolTip = "Optional feet/base offset. Most actors can leave this at zero because Auto origin uses the bottom of the visual bounds."))
	FVector SortOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	ESpriteSortOriginMode OriginMode = ESpriteSortOriginMode::Auto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	FVector SortAxis = FVector(0.f, 1.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	FVector CameraDepthAxis = FVector(0.f, 0.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (EditCondition = "OriginMode == ESpriteSortOriginMode::Auto || OriginMode == ESpriteSortOriginMode::VisualBoundsBase || OriginMode == ESpriteSortOriginMode::TargetBoundsBase", EditConditionHides))
	FVector BoundsBaseAxis = FVector(0.f, 0.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.000001"))
	float DepthScale = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ToolTip = "Keeps gameplay Z from accidentally overpowering top-down XY sorting. Visuals keep their local offsets, but actor/world height is flattened before the sort depth is applied."))
	bool bIgnoreActorDepth = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.0", ToolTip = "Base render-depth lift applied to sortable actors so they stay above unsorted ground/map sprites."))
	float GroundDepthBias = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ToolTip = "Prevents sortable actors from being pushed behind unsorted ground/map sprites when the sort value goes negative."))
	bool bKeepAboveGroundPlane = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.0", ToolTip = "Minimum depth separation from unsorted ground/map sprites when bKeepAboveGroundPlane is enabled."))
	float MinimumGroundSeparation = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.0", ToolTip = "Snaps render-depth offsets to a small grid to reduce depth-buffer flicker when two masked sprites are almost coplanar. Set to 0 to disable."))
	float DepthSnapInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.0", ToolTip = "Tiny extra spacing on the render-depth axis. Useful for keeping props a hair above ground tiles without hardcoding actor Z."))
	float DepthPadding = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ToolTip = "When true, SpriteSort2D moves every safe visual primitive on the actor, including sibling sprites for clothes, hair, weapons, shadows, and attachments. Collision is ignored."))
	bool bSortAllVisualComponents = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Performance", meta = (ClampMin = "0.0"))
	float MovementThreshold = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Performance", meta = (ClampMin = "0.0"))
	float WhenMovedTickInterval = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	bool bInvertSort = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	bool bEnableSorting = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	bool bDebugDraw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	ESpriteSortUpdateMode UpdateMode = ESpriteSortUpdateMode::SmartAuto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	ESpriteSortMode SortingMode = ESpriteSortMode::VisualDepthOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Auto Find")
	bool bAutoFindVisualRoot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort|Auto Find")
	bool bAutoFindTargetPrimitive = true;

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void UpdateSortNow();

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void SetSortingEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void SetVisualRoot(USceneComponent* NewVisualRoot);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void SetSortOrigin(USceneComponent* NewSortOrigin);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void SetSortOffset(FVector NewOffset);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void SetOriginMode(ESpriteSortOriginMode NewOriginMode);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void SetUpdateMode(ESpriteSortUpdateMode NewUpdateMode);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	float GetCurrentSortValue() const;

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	FVector GetSortWorldLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	FVector GetCurrentVisualDepthOffset() const;

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	void RestoreOriginalVisualTransform();

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort|Auto Find")
	void AutoFindVisualRoot();

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort|Auto Find")
	void AutoFindTargetPrimitive();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	FTransform OriginalRelativeTransform = FTransform::Identity;
	bool bHasOriginalRelativeTransform = false;
	float CurrentSortValue = 0.f;
	FVector CurrentVisualDepthOffset = FVector::ZeroVector;
	FVector LastOwnerLocation = FVector::ZeroVector;
	FVector LastSortWorldLocation = FVector::ZeroVector;
	bool bHasMovementBaseline = false;
	TMap<TWeakObjectPtr<USceneComponent>, FTransform> OriginalRelativeTransforms;

	void RefreshVisualComponentCache();
	void CacheOriginalVisualTransform(bool bForce = false);
	void ResetMovementBaseline();
	void SetMovementBaseline(FVector NewSortWorldLocation);
	bool HasSortMovementChanged();
	bool ShouldUseMovementUpdates() const;
	bool ValidateForSorting() const;
	void ApplyVisualDepthOffset();
	void ApplyDepthOffsetToComponent(USceneComponent* Component, const FTransform& OriginalTransform);
	float GetDepthOffsetScalar() const;
	void ApplyTranslucentPriorityFallback();
	void DrawDebugInfo();
	void WarnIfVisualRootContainsCollision() const;
	void UpdateTickEnabled();
	ESpriteSortUpdateMode GetResolvedUpdateMode() const;
	bool IsProbablyDynamicActor() const;
	bool UsesBoundsBasedOrigin() const;
	FVector GetStableSortWorldLocation();
	bool TryGetVisualBounds(FBoxSphereBounds& OutBounds) const;
	bool TryGetTargetBounds(FBoxSphereBounds& OutBounds) const;
	FVector GetActorLocationPlusOffset() const;
};
