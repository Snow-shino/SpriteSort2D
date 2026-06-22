#include "SpriteSortComponentCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "SpriteSortComponent.h"
#include "GameFramework/Actor.h"
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

	Category.AddCustomRow(LOCTEXT("SpriteSortWarnings", "Sprite Sort Warnings"))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Text(this, &FSpriteSortComponentCustomization::GetWarningText)
		.Visibility(this, &FSpriteSortComponentCustomization::GetWarningVisibility)
		.ColorAndOpacity(FLinearColor(1.f, 0.55f, 0.15f, 1.f))
		.AutoWrapText(true)
	];
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

		if (!Component->VisualRoot && !Component->bSortAllVisualComponents)
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

#undef LOCTEXT_NAMESPACE
