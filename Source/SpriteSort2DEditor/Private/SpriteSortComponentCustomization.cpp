#include "SpriteSortComponentCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Engine/Blueprint.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "ScopedTransaction.h"
#include "SpriteSortComponent.h"
#include "SpriteSortFunctionLibrary.h"
#include "SpriteSortOriginComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SpriteSortComponentCustomization"

TSharedRef<IDetailCustomization> FSpriteSortComponentCustomization::MakeInstance()
{
	return MakeShared<FSpriteSortComponentCustomization>();
}

void FSpriteSortComponentCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	Components.Reset();
	for (const TWeakObjectPtr<UObject>& Object : Objects)
	{
		if (USpriteSortComponent* Component = Cast<USpriteSortComponent>(Object.Get()))
		{
			Components.Add(Component);
		}
	}

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(TEXT("Sprite Sort"));

	Category.AddCustomRow(LOCTEXT("SpriteSortActions", "Sprite Sort Actions"))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(SButton)
			.Text(LOCTEXT("UpdateSortNow", "Update Sort Now"))
			.OnClicked(this, &FSpriteSortComponentCustomization::UpdateSortNow)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(SButton)
			.Text(LOCTEXT("AutoFindVisualRoot", "Auto Find Visual Root"))
			.OnClicked(this, &FSpriteSortComponentCustomization::AutoFindVisualRoot)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("CreateSortOrigin", "Create Sort Origin"))
			.OnClicked(this, &FSpriteSortComponentCustomization::CreateSortOrigin)
		]
	];

	Category.AddCustomRow(LOCTEXT("SpriteSortWarnings", "Sprite Sort Warnings"))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Text(this, &FSpriteSortComponentCustomization::GetWarningText)
		.ColorAndOpacity(FLinearColor(1.f, 0.55f, 0.15f, 1.f))
		.AutoWrapText(true)
	];
}

FReply FSpriteSortComponentCustomization::UpdateSortNow()
{
	for (const TWeakObjectPtr<USpriteSortComponent>& ComponentPtr : Components)
	{
		if (USpriteSortComponent* Component = ComponentPtr.Get())
		{
			Component->Modify();
			if (Component->GetOwner())
			{
				Component->UpdateSortNow();
			}
			else
			{
				AutoFindOnBlueprintTemplate(Component);
			}
		}
	}

	return FReply::Handled();
}

FReply FSpriteSortComponentCustomization::AutoFindVisualRoot()
{
	const FScopedTransaction Transaction(LOCTEXT("AutoFindVisualRootTransaction", "Auto Find Sprite Sort Visual Root"));

	for (const TWeakObjectPtr<USpriteSortComponent>& ComponentPtr : Components)
	{
		if (USpriteSortComponent* Component = ComponentPtr.Get())
		{
			Component->Modify();
			if (Component->GetOwner())
			{
				Component->AutoFindVisualRoot();
				Component->AutoFindTargetPrimitive();
				Component->UpdateSortNow();
			}
			else
			{
				AutoFindOnBlueprintTemplate(Component);
			}
		}
	}

	return FReply::Handled();
}

FReply FSpriteSortComponentCustomization::CreateSortOrigin()
{
	const FScopedTransaction Transaction(LOCTEXT("CreateSortOriginTransaction", "Create Sprite Sort Origin"));

	for (const TWeakObjectPtr<USpriteSortComponent>& ComponentPtr : Components)
	{
		USpriteSortComponent* Component = ComponentPtr.Get();
		AActor* Owner = Component ? Component->GetOwner() : nullptr;
		USceneComponent* RootComponent = Owner ? Owner->GetRootComponent() : nullptr;
		if (!Component || !Owner || !RootComponent)
		{
			if (Component)
			{
				CreateSortOriginOnBlueprintTemplate(Component);
			}
			continue;
		}

		Owner->Modify();
		Component->Modify();

		USpriteSortOriginComponent* Origin = NewObject<USpriteSortOriginComponent>(Owner, USpriteSortOriginComponent::StaticClass(), TEXT("SpriteSortOrigin"), RF_Transactional);
		Origin->SetupAttachment(RootComponent);
		Origin->SetRelativeLocation(Component->SortOffset);
		Owner->AddInstanceComponent(Origin);
		Origin->RegisterComponent();

		Component->SetSortOrigin(Origin);
		Component->SortOffset = FVector::ZeroVector;
	}

	return FReply::Handled();
}

FText FSpriteSortComponentCustomization::GetWarningText() const
{
	for (const TWeakObjectPtr<USpriteSortComponent>& ComponentPtr : Components)
	{
		const USpriteSortComponent* Component = ComponentPtr.Get();
		const AActor* Owner = Component ? Component->GetOwner() : nullptr;
		if (!Component)
		{
			continue;
		}

		if (!Component->VisualRoot)
		{
			return LOCTEXT("MissingVisualRootWarning", "VisualRoot is missing. Assign one or use Auto Find Visual Root.");
		}

		if (Owner && Component->VisualRoot == Owner->GetRootComponent())
		{
			return LOCTEXT("RootVisualWarning", "VisualRoot is the actor root. Use a child VisualRoot so collision and gameplay transforms stay fixed.");
		}

		if (Component->SortOrigin && Component->VisualRoot && Component->SortOrigin->IsAttachedTo(Component->VisualRoot))
		{
			return LOCTEXT("SortOriginUnderVisualRootWarning", "SortOrigin is attached under VisualRoot. Move it outside VisualRoot so sorting does not move the origin.");
		}

		if (Component->SortAxis.IsNearlyZero())
		{
			return LOCTEXT("ZeroSortAxisWarning", "SortAxis is zero. Use a non-zero axis such as world Y.");
		}

		if (Component->CameraDepthAxis.IsNearlyZero())
		{
			return LOCTEXT("ZeroDepthAxisWarning", "CameraDepthAxis is zero. Use the axis that should receive visual depth offsets.");
		}

		if ((Component->OriginMode == ESpriteSortOriginMode::Auto || Component->OriginMode == ESpriteSortOriginMode::VisualBoundsBase || Component->OriginMode == ESpriteSortOriginMode::TargetBoundsBase) && Component->BoundsBaseAxis.IsNearlyZero())
		{
			return LOCTEXT("ZeroBoundsBaseAxisWarning", "BoundsBaseAxis is zero. Use the axis from sprite base toward sprite top, usually world Z.");
		}
	}

	return FText::GetEmpty();
}

EVisibility FSpriteSortComponentCustomization::GetWarningVisibility() const
{
	return GetWarningText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

bool FSpriteSortComponentCustomization::AutoFindOnBlueprintTemplate(USpriteSortComponent* Component) const
{
	if (!Component)
	{
		return false;
	}

	USCS_Node* ComponentNode = FindNodeForTemplate(Component);
	USimpleConstructionScript* SCS = ComponentNode ? ComponentNode->GetSCS() : nullptr;
	UBlueprint* Blueprint = SCS ? SCS->GetBlueprint() : GetBlueprintForTemplate(Component);
	if (!SCS || !Blueprint)
	{
		return false;
	}

	SCS->Modify();
	Component->Modify();

	if (!Component->VisualRoot)
	{
		Component->VisualRoot = FindBestBlueprintVisualRoot(SCS);
	}

	if (!Component->TargetPrimitive)
	{
		Component->TargetPrimitive = FindBestBlueprintTargetPrimitive(SCS, Component->VisualRoot);
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	Component->PostEditChange();
	return Component->VisualRoot != nullptr;
}

bool FSpriteSortComponentCustomization::CreateSortOriginOnBlueprintTemplate(USpriteSortComponent* Component) const
{
	if (!Component)
	{
		return false;
	}

	USCS_Node* ComponentNode = FindNodeForTemplate(Component);
	USimpleConstructionScript* SCS = ComponentNode ? ComponentNode->GetSCS() : nullptr;
	UBlueprint* Blueprint = SCS ? SCS->GetBlueprint() : GetBlueprintForTemplate(Component);
	if (!SCS || !Blueprint)
	{
		return false;
	}

	SCS->Modify();
	Component->Modify();

	const FName OriginName = SCS->GenerateNewComponentName(USpriteSortOriginComponent::StaticClass(), TEXT("SpriteSortOrigin"));
	USCS_Node* OriginNode = SCS->CreateNode(USpriteSortOriginComponent::StaticClass(), OriginName);
	USpriteSortOriginComponent* OriginTemplate = OriginNode ? Cast<USpriteSortOriginComponent>(OriginNode->ComponentTemplate) : nullptr;
	if (!OriginNode || !OriginTemplate)
	{
		return false;
	}

	OriginTemplate->SetRelativeLocation(Component->SortOffset);

	USCS_Node* RootNode = nullptr;
	SCS->GetSceneRootComponentTemplate(true, &RootNode);
	if (RootNode)
	{
		RootNode->AddChildNode(OriginNode);
	}
	else
	{
		if (Blueprint->GeneratedClass)
		{
			if (const AActor* CDO = Cast<AActor>(Blueprint->GeneratedClass->GetDefaultObject()))
			{
				if (const USceneComponent* NativeRoot = CDO->GetRootComponent())
				{
					OriginNode->SetParent(NativeRoot);
				}
			}
		}
		SCS->AddNode(OriginNode);
	}

	Component->SortOrigin = OriginTemplate;
	Component->SortOffset = FVector::ZeroVector;

	if (!Component->VisualRoot)
	{
		Component->VisualRoot = FindBestBlueprintVisualRoot(SCS);
	}

	if (!Component->TargetPrimitive)
	{
		Component->TargetPrimitive = FindBestBlueprintTargetPrimitive(SCS, Component->VisualRoot);
	}

	SCS->ValidateSceneRootNodes();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	Component->PostEditChange();
	return true;
}

UBlueprint* FSpriteSortComponentCustomization::GetBlueprintForTemplate(const USpriteSortComponent* Component) const
{
	if (!Component)
	{
		return nullptr;
	}

	if (const USCS_Node* Node = Component->GetTypedOuter<USCS_Node>())
	{
		return Node->GetSCS() ? Node->GetSCS()->GetBlueprint() : nullptr;
	}

	return Component->GetTypedOuter<UBlueprint>();
}

USCS_Node* FSpriteSortComponentCustomization::FindNodeForTemplate(const USpriteSortComponent* Component) const
{
	if (!Component)
	{
		return nullptr;
	}

	if (USCS_Node* DirectNode = Component->GetTypedOuter<USCS_Node>())
	{
		return DirectNode;
	}

	if (UBlueprint* Blueprint = GetBlueprintForTemplate(Component))
	{
		if (USimpleConstructionScript* SCS = Blueprint->SimpleConstructionScript)
		{
			for (USCS_Node* Node : SCS->GetAllNodes())
			{
				if (Node && Node->ComponentTemplate == Component)
				{
					return Node;
				}
			}
		}
	}

	return nullptr;
}

USceneComponent* FSpriteSortComponentCustomization::FindBestBlueprintVisualRoot(USimpleConstructionScript* SCS) const
{
	if (!SCS)
	{
		return nullptr;
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		USceneComponent* SceneComponent = Node ? Cast<USceneComponent>(Node->ComponentTemplate) : nullptr;
		if (SceneComponent && Node->GetVariableName().IsEqual(TEXT("VisualRoot"), ENameCase::IgnoreCase))
		{
			return SceneComponent;
		}
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		if (UPaperFlipbookComponent* Flipbook = Node ? Cast<UPaperFlipbookComponent>(Node->ComponentTemplate) : nullptr)
		{
			return Flipbook;
		}
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		if (UPaperSpriteComponent* Sprite = Node ? Cast<UPaperSpriteComponent>(Node->ComponentTemplate) : nullptr)
		{
			return Sprite;
		}
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		UPrimitiveComponent* Primitive = Node ? Cast<UPrimitiveComponent>(Node->ComponentTemplate) : nullptr;
		if (Primitive && !USpriteSortFunctionLibrary::IsLikelyCollisionComponent(Primitive))
		{
			return Primitive;
		}
	}

	return nullptr;
}

UPrimitiveComponent* FSpriteSortComponentCustomization::FindBestBlueprintTargetPrimitive(USimpleConstructionScript* SCS, USceneComponent* VisualRoot) const
{
	if (!SCS)
	{
		return nullptr;
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		if (UPaperFlipbookComponent* Flipbook = Node ? Cast<UPaperFlipbookComponent>(Node->ComponentTemplate) : nullptr)
		{
			return Flipbook;
		}
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		if (UPaperSpriteComponent* Sprite = Node ? Cast<UPaperSpriteComponent>(Node->ComponentTemplate) : nullptr)
		{
			return Sprite;
		}
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		UPrimitiveComponent* Primitive = Node ? Cast<UPrimitiveComponent>(Node->ComponentTemplate) : nullptr;
		if (!Primitive || USpriteSortFunctionLibrary::IsLikelyCollisionComponent(Primitive))
		{
			continue;
		}

		if (!VisualRoot || Primitive == VisualRoot || Primitive->IsAttachedTo(VisualRoot))
		{
			return Primitive;
		}
	}

	for (USCS_Node* Node : SCS->GetAllNodes())
	{
		UPrimitiveComponent* Primitive = Node ? Cast<UPrimitiveComponent>(Node->ComponentTemplate) : nullptr;
		if (Primitive && !USpriteSortFunctionLibrary::IsLikelyCollisionComponent(Primitive))
		{
			return Primitive;
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
