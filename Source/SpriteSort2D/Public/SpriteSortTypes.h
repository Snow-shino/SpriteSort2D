#pragma once

#include "CoreMinimal.h"
#include "SpriteSortTypes.generated.h"

UENUM(BlueprintType)
enum class ESpriteSortUpdateMode : uint8
{
	OnBeginPlayOnly UMETA(DisplayName = "On Begin Play Only"),
	WhenMoved UMETA(DisplayName = "When Moved"),
	EveryTick UMETA(DisplayName = "Every Tick"),
	Manual UMETA(DisplayName = "Manual")
};

UENUM(BlueprintType)
enum class ESpriteSortMode : uint8
{
	VisualDepthOffset UMETA(DisplayName = "Visual Depth Offset"),
	Manual UMETA(DisplayName = "Manual"),
	TranslucentPriorityFallback UMETA(DisplayName = "Translucent Priority Fallback")
};

UENUM(BlueprintType)
enum class ESpriteSortOriginMode : uint8
{
	Auto UMETA(DisplayName = "Auto"),
	ExplicitSortOrigin UMETA(DisplayName = "Explicit Sort Origin"),
	ActorLocationPlusOffset UMETA(DisplayName = "Actor Location + Offset"),
	VisualBoundsBase UMETA(DisplayName = "Visual Bounds Base"),
	TargetBoundsBase UMETA(DisplayName = "Target Bounds Base")
};
