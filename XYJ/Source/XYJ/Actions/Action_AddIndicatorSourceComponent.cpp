#include "Action_AddIndicatorSourceComponent.h"
#include "CancerIndicatorSourceComponent.h"

void UAction_AddIndicatorSourceComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GatherSoftReferences(OutPaths);
}

void UAction_AddIndicatorSourceComponent::PostInitComponent_Implementation()
{
	UCancerIndicatorSourceComponent* IndicatorSourceComponent = Cast<UCancerIndicatorSourceComponent>(
		GetCreateComponent());
	check(IndicatorSourceComponent);

	IndicatorSourceComponent->bVisible = bVisible;
	IndicatorSourceComponent->IndicatorWidgetClass = IndicatorWidgetClass;
	IndicatorSourceComponent->IndicatorComponentName = IndicatorComponentName;
	IndicatorSourceComponent->ComponentSocketName = ComponentSocketName;
	IndicatorSourceComponent->bClampToScreen = bClampToScreen;
	IndicatorSourceComponent->MaxDis = MaxDis;
	IndicatorSourceComponent->bShowClampToScreenArrow = bShowClampToScreenArrow;
	IndicatorSourceComponent->bAutoRemoveWhenIndicatorComponentIsNull = bAutoRemoveWhenIndicatorComponentIsNull;
	IndicatorSourceComponent->ProjectionMode = ProjectionMode;
	IndicatorSourceComponent->HAlignment = HAlignment;
	IndicatorSourceComponent->VAlignment = VAlignment;
	IndicatorSourceComponent->Priority = Priority;
	IndicatorSourceComponent->BoundingBoxAnchor = BoundingBoxAnchor;
	IndicatorSourceComponent->ScreenSpaceOffset = ScreenSpaceOffset;
	IndicatorSourceComponent->WorldPositionOffset = WorldPositionOffset;
	IndicatorSourceComponent->bAutoUpdateVisible = bAutoUpdateVisible;
}
