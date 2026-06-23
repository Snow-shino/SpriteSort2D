#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpriteSortOriginComponent.generated.h"

UCLASS(ClassGroup = (SpriteSort2D), meta = (BlueprintSpawnableComponent, DisplayName = "Sprite Sort Origin"))
class SPRITESORT2D_API USpriteSortOriginComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USpriteSortOriginComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Sort", meta = (ToolTip = "When true, SpriteSortComponent Auto mode uses this component as the actor's explicit feet/base sort line. Move it up/down on the map axis to tune when players go behind this prop."))
	bool bAutoUseAsSortOrigin = true;
};
