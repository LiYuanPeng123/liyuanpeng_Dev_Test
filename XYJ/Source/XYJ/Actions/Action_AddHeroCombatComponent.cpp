#include "Action_AddHeroCombatComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/CancerHeroComponent.h"

void UAction_AddHeroCombatComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!BossWidgetClass.IsNull())
	{
		OutPaths.Add(BossWidgetClass.ToSoftObjectPath());
	}
}

void UAction_AddHeroCombatComponent::PostInitComponent_Implementation()
{
	UCancerHeroComponent* HeroComponent = Cast<UCancerHeroComponent>(GetCreateComponent());
	check(HeroComponent);
	HeroComponent->bIsShowBossWidget = bIsShowBossWidget;
	HeroComponent->BossWidgetClass = BossWidgetClass.Get();
	HeroComponent->SlotID = SlotID;
	HeroComponent->TriggerTags = TriggerTags;
	HeroComponent->ExitCombatTime = ExitCombatTime;
	HeroComponent->ExitTags = ExitTags;
	HeroComponent->ComboTag = ComboTag;
}
