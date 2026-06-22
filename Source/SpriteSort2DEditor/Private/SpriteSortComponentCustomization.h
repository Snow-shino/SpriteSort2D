#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtrTemplates.h"

class USpriteSortComponent;
class UBlueprint;
class UPrimitiveComponent;
class USceneComponent;
class USCS_Node;
class USimpleConstructionScript;

class FSpriteSortComponentCustomization final : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TArray<TWeakObjectPtr<USpriteSortComponent>> Components;

	FReply UpdateSortNow();
	FReply AutoFindVisualRoot();
	FReply CreateSortOrigin();
	FText GetWarningText() const;
	EVisibility GetWarningVisibility() const;

	bool AutoFindOnBlueprintTemplate(USpriteSortComponent* Component) const;
	bool CreateSortOriginOnBlueprintTemplate(USpriteSortComponent* Component) const;
	UBlueprint* GetBlueprintForTemplate(const USpriteSortComponent* Component) const;
	USCS_Node* FindNodeForTemplate(const USpriteSortComponent* Component) const;
	USceneComponent* FindBestBlueprintVisualRoot(USimpleConstructionScript* SCS) const;
	UPrimitiveComponent* FindBestBlueprintTargetPrimitive(USimpleConstructionScript* SCS, USceneComponent* VisualRoot) const;
};
