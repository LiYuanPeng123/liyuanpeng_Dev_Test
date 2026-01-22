#include "CancerIndicatorSourceComponent.h"

#include "CancerIndicatorSubsystem.h"


class UCancerIndicatorSubsystem;

UCancerIndicatorSourceComponent::UCancerIndicatorSourceComponent()
{
}

void UCancerIndicatorSourceComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		if (UCancerIndicatorSubsystem* IndicatorSubsystem = GetWorld()->GetSubsystem<UCancerIndicatorSubsystem>())
		{
			IndicatorSubsystem->AddIndicator(this);
		}
	}
}

void UCancerIndicatorSourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		if (UCancerIndicatorSubsystem* IndicatorSubsystem = GetWorld()->GetSubsystem<UCancerIndicatorSubsystem>())
		{
			IndicatorSubsystem->RemoveIndicator(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

UIndicatorDescriptor* UCancerIndicatorSourceComponent::ConstructIndicator()
{
	Guid = FGuid::NewGuid();
	auto Indicator = NewObject<UIndicatorDescriptor>(GetOwner());
	Indicator->SetIndicatorClass(IndicatorWidgetClass);
	auto SceneComponent = GetOwner()->FindComponentByTag<USceneComponent>(IndicatorComponentName);
	check(SceneComponent);
	Indicator->SetSceneComponent(SceneComponent);
	Indicator->SetProjectionMode(ProjectionMode);
	Indicator->SetWorldPositionOffset(WorldPositionOffset);
	Indicator->SetDataObject(GetOwner());
	Indicator->SetComponentSocketName(ComponentSocketName);
	Indicator->SetAutoRemoveWhenIndicatorComponentIsNull(bAutoRemoveWhenIndicatorComponentIsNull);
	Indicator->SetDesiredVisibility(bVisible);
	Indicator->SetHAlign(HAlignment);
	Indicator->SetVAlign(VAlignment);
	Indicator->SetClampToScreen(bClampToScreen);
	Indicator->SetShowClampToScreenArrow(bShowClampToScreenArrow);
	Indicator->SetScreenSpaceOffset(ScreenSpaceOffset);
	Indicator->SetBoundingBoxAnchor(BoundingBoxAnchor);
	Indicator->SetMaxDis(MaxDis);
	Indicator->SetAutoUpdateVisible(bAutoUpdateVisible);
	Indicator->SetPriority(Priority);
	SourceIndicatorDescriptor = Indicator;
	return SourceIndicatorDescriptor;
}

UIndicatorDescriptor* UCancerIndicatorSourceComponent::GetIndicator()
{
	if (SourceIndicatorDescriptor)
		return SourceIndicatorDescriptor;
	SourceIndicatorDescriptor = ConstructIndicator();
	return SourceIndicatorDescriptor;
}
