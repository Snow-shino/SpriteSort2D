#include "SpriteSortOriginComponent.h"

USpriteSortOriginComponent::USpriteSortOriginComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
#if WITH_EDITORONLY_DATA
	bVisualizeComponent = true;
#endif
	SetMobility(EComponentMobility::Movable);
}
