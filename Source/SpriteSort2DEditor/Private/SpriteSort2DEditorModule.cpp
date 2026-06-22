#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "SpriteSortComponent.h"
#include "SpriteSortComponentCustomization.h"

class FSpriteSort2DEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.RegisterCustomClassLayout(
			USpriteSortComponent::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&FSpriteSortComponentCustomization::MakeInstance));
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}

	virtual void ShutdownModule() override
	{
		if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
		{
			FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
			PropertyEditorModule.UnregisterCustomClassLayout(USpriteSortComponent::StaticClass()->GetFName());
			PropertyEditorModule.NotifyCustomizationModuleChanged();
		}
	}
};

IMPLEMENT_MODULE(FSpriteSort2DEditorModule, SpriteSort2DEditor)
