#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SpriteSortTypes.h"
#include "SpriteSortSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Sprite Sort 2D"))
class SPRITESORT2D_API USpriteSortSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USpriteSortSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	FVector DefaultSortAxis = FVector(0.f, 1.f, 0.f);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	FVector DefaultCameraDepthAxis = FVector(0.f, 0.f, 1.f);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	bool bDefaultUseCameraForwardDepthAxis = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	FVector DefaultBoundsBaseAxis = FVector(0.f, 0.f, 1.f);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults", meta = (ClampMin = "0.000001"))
	float DefaultDepthScale = 0.01f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (ClampMin = "0.0"))
	float DefaultMovementThreshold = 0.1f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (ClampMin = "0.0"))
	float DefaultWhenMovedTickInterval = 0.1f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	ESpriteSortUpdateMode DefaultUpdateMode = ESpriteSortUpdateMode::SmartAuto;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	ESpriteSortOriginMode DefaultOriginMode = ESpriteSortOriginMode::Auto;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	bool bDebugDrawByDefault = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Auto Find")
	bool bAutoFindVisualRoot = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Auto Find")
	bool bAutoFindTargetPrimitive = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Auto Find")
	bool bAutoFindSortOrigin = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Auto Find")
	bool bSortAllVisualComponents = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults", meta = (ClampMin = "0.0"))
	float DefaultDepthPadding = 0.2f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Defaults", meta = (ClampMin = "0.0"))
	float DefaultForegroundDepthBias = 100.f;

	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
};
