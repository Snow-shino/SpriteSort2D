#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpriteSortFunctionLibrary.generated.h"

class UPrimitiveComponent;
class USceneComponent;

UCLASS()
class SPRITESORT2D_API USpriteSortFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Sprite Sort")
	static float CalculateSortValueFromLocation(FVector WorldLocation, FVector SortAxis, bool bInvertSort = false);

	UFUNCTION(BlueprintPure, Category = "Sprite Sort")
	static FVector CalculateDepthOffset(float SortValue, FVector CameraDepthAxis, float DepthScale);

	UFUNCTION(BlueprintPure, Category = "Sprite Sort")
	static FVector CalculateBoundsBaseLocation(FBoxSphereBounds Bounds, FVector BoundsBaseAxis, FVector SortOffset = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort")
	static void ApplyVisualDepthOffset(USceneComponent* VisualRoot, FTransform OriginalRelativeTransform, FVector DepthOffset);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort", meta = (DefaultToSelf = "Owner"))
	static USceneComponent* FindBestPaper2DVisualComponent(AActor* Owner);

	UFUNCTION(BlueprintCallable, Category = "Sprite Sort", meta = (DefaultToSelf = "Owner"))
	static UPrimitiveComponent* FindBestPrimitiveTarget(AActor* Owner, USceneComponent* VisualRoot);

	static bool IsLikelyCollisionComponent(const UPrimitiveComponent* Primitive);
};
