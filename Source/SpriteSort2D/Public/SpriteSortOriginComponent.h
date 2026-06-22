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
};
