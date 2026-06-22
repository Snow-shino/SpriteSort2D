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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	TObjectPtr<UPrimitiveComponent> TargetPrimitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
	TObjectPtr<USceneComponent> SortOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort")
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
	float DepthScale = 0.01f;

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
	ESpriteSortUpdateMode UpdateMode = ESpriteSortUpdateMode::WhenMoved;

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

	void CacheOriginalVisualTransform(bool bForce = false);
	void ResetMovementBaseline();
	void SetMovementBaseline(FVector NewSortWorldLocation);
	bool HasSortMovementChanged();
	bool ValidateForSorting() const;
	void ApplyVisualDepthOffset();
	void ApplyTranslucentPriorityFallback();
	void DrawDebugInfo();
	void WarnIfVisualRootContainsCollision() const;
	void UpdateTickEnabled();
	bool UsesBoundsBasedOrigin() const;
	FVector GetStableSortWorldLocation();
	bool TryGetVisualBounds(FBoxSphereBounds& OutBounds) const;
	bool TryGetTargetBounds(FBoxSphereBounds& OutBounds) const;
	FVector GetActorLocationPlusOffset() const;
};
