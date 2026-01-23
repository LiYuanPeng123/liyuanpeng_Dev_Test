#include "Action_AddAbilitySystemComponent.h"

#include "CancerAbilityFunctionLibrary.h"
#include "CancerCameraBehaviorTreeComponent.h"
#include "CancerCameraBlueprintLibrary.h"
#include "CancerInputFunctionLibrary.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerQuickBarInstance.h"
#include "CancerAbility/Public/CancerAbilitySystemComponent.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Components/CancerInputManagerComponent.h"
#include "Components/CancerQuickBarComponent.h"
#include "Data/CancerCombatData.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "CancerNativeGamePlayTag.h"
#include "Chooser.h"
#include "Components/CancerHeroComponent.h"

void UAction_AddAbilitySystemComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!AbilityData.IsNull())
		OutPaths.Add(AbilityData.ToSoftObjectPath());
	if (!SpecailAbilityData.IsNull())
	{
		OutPaths.Add(SpecailAbilityData.ToSoftObjectPath());
	}
	if (!StaggerAnimationData.IsNull())
		OutPaths.Add(StaggerAnimationData.ToSoftObjectPath());

	if (!ChooserTable.IsNull())
		OutPaths.Add(ChooserTable.ToSoftObjectPath());

	if (!StartAbilitySet.IsNull())
	{
		OutPaths.Add(StartAbilitySet.ToSoftObjectPath());
	}
	if (!CombatData.IsNull())
	{
		OutPaths.Add(CombatData.ToSoftObjectPath());
	}
}

void UAction_AddAbilitySystemComponent::PostInitComponent_Implementation()
{
	UCancerAbilitySystemComponent* ASC = Cast<UCancerAbilitySystemComponent>(GetCreateComponent());
	check(ASC);
	

	//刷新技能数据
	ASC->SetAbilitiesData(AbilityData.Get());

	ASC->SetSpecialAbilityData(SpecailAbilityData.Get());
	
	ASC->SetAttributeSetData(ComboAttributeSet);

	ASC->SetLevelEffect(LevelGE);

	ASC->SetStaggerAnimData(StaggerAnimationData.Get());
	
	//刷新技能蒙太奇
	ASC->RefreshAbilityMontageMapFromTables(ChooserTable.Get());

	

	//设置战斗数据
	UCancerCombatData* CombatDataPtr = CombatData.Get();
	ASC->SetCombatData(CombatDataPtr);
}

void UAction_AddAbilitySystemComponent::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();
	UCancerAbilitySystemComponent* ASC = Cast<UCancerAbilitySystemComponent>(GetCreateComponent());
	check(ASC);

	AActor* AvatarActor = GetCreateComponent()->GetOwner();
	ASC->InitAbilityActorInfo(AvatarActor, AvatarActor);

	if (UCancerAbilitySet* StartSet = StartAbilitySet.Get())
	{
		StartSet->GiveToAbilitySystem(ASC, nullptr, AvatarActor);
	}

	if (UCancerInputManagerComponent* InputManager =
		UCancerInputFunctionLibrary::GetCancerInputManagerComponent(AvatarActor))
	{
		InputManager->OnReleased.AddDynamic(ASC, &UCancerAbilitySystemComponent::AbilityInputTagReleased);
		InputManager->OnPressed.AddDynamic(ASC, &UCancerAbilitySystemComponent::AbilityInputTagPressed);
	}
	
	UCancerCombatAttributeComponent* CombatAttribute =
		UCancerCombatAttributeComponent::FindCancerCombatAttributeComponent(GetCreatActor());
	if (CombatAttribute)
		CombatAttribute->InitializeWithAbilitySystem(ASC);

	//刷新一次等级
	ASC->SetLevel(ASC->GetLevel());
	
	if (auto HeroComponent =
		UCancerAbilityFunctionLibrary::GetCancerCombatManagerComponent(AvatarActor))
	{
		HeroComponent->InitializeWithAbilitySystem();
		
		if (auto CameraComponent =
			UCancerCameraBlueprintLibrary::GetCancerCameraBehaviorTreeComponent(AvatarActor))
		{
			CameraComponent->DetermineCameraModeDelegate.
			                 BindUObject(HeroComponent, &UCancerHeroComponent::DetermineCameraMode);
		}
	}
}
