#include "SpriteSortSettings.h"

USpriteSortSettings::USpriteSortSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("SpriteSort2D");
}

FName USpriteSortSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FName USpriteSortSettings::GetSectionName() const
{
	return TEXT("SpriteSort2D");
}
