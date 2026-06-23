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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ToolTip = "Uses the active camera forward vector for the invisible render-depth push. This is recommended for full games because sprite height/Z placement should not decide sorting. CameraDepthAxis is used as a fallback when no camera is available."))
	bool bUseCameraForwardDepthAxis = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (EditCondition = "OriginMode == ESpriteSortOriginMode::VisualBoundsBase || OriginMode == ESpriteSortOriginMode::TargetBoundsBase", EditConditionHides))
	FVector BoundsBaseAxis = FVector(0.f, 0.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.000001"))
	float DepthScale = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.0", ToolTip = "Tiny extra spacing on the render-depth axis. This is added smoothly and does not snap or clamp actor Z."))
	float DepthPadding = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ClampMin = "0.0", ToolTip = "Keeps sorted actors in front of unsorted background/floor sprites by moving visuals slightly toward the camera before fine prop sorting is applied. Increase if characters clip behind the floor; lower if they get too close to the camera near plane."))
	float ForegroundDepthBias = 100.f;

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
	TArray<TWeakObjectPtr<UPrimitiveComponent>> VisualBoundsComponents;

	void RefreshVisualComponentCache();
	void CacheOriginalVisualTransform(bool bForce = false);
	void ResetMovementBaseline();
	void SetMovementBaseline(FVector NewSortWorldLocation);
	bool HasSortMovementChanged();
	bool ShouldUseMovementUpdates() const;
	bool ValidateForSorting() const;
	void ApplyVisualDepthOffset();
	void ApplyDepthOffsetToComponent(USceneComponent* Component, const FTransform& OriginalTransform);
	void ApplyTranslucentPriorityFallback();
	void DrawDebugInfo();
	void WarnIfVisualRootContainsCollision() const;
	void UpdateTickEnabled();
	ESpriteSortUpdateMode GetResolvedUpdateMode() const;
	bool IsProbablyDynamicActor() const;
	bool UsesBoundsBasedOrigin() const;
	FVector GetStableSortWorldLocation();
	FVector GetEffectiveCameraDepthAxis() const;
	bool TryGetVisualPivotLocation(FVector& OutLocation) const;
	bool TryGetVisualBounds(FBoxSphereBounds& OutBounds) const;
	bool TryGetTargetBounds(FBoxSphereBounds& OutBounds) const;
	FVector GetActorLocationPlusOffset() const;
};
