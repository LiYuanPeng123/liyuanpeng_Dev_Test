
#include "Animation/AnimNotify_ApplyEffectToBothSide.h"

#include "AbilitySystemGlobals.h"
#include "CancerAbilitySystemComponent.h"


void UAnimNotify_ApplyEffectToBothSide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
											   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (UCancerAbilitySystemComponent* CancerAbilitySystemComponent = MeshComp->GetOwner()->FindComponentByClass<UCancerAbilitySystemComponent>())
	{
		
		if (AActor* HitSourceActor = CancerAbilitySystemComponent->CurrentHitSource.Get())
		{
			UAbilitySystemComponent* HitSourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitSourceActor);
			for (auto GEClass : ApplyToAttackerGE)
			{
				CancerAbilitySystemComponent->ApplyGameplayEffectToTarget(
				GEClass.GetDefaultObject(),
				HitSourceASC,
				1, 
				CancerAbilitySystemComponent->MakeEffectContext());
			}
		}

		if (AActor* HitTargetActor = CancerAbilitySystemComponent->CurrentHitTarget.Get())
		{
			UAbilitySystemComponent* HitTargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitTargetActor);
			for (auto GEClass : ApplyToHitTargetGE)
			{
				CancerAbilitySystemComponent->ApplyGameplayEffectToTarget(
				GEClass.GetDefaultObject(),
				HitTargetASC,
				1, 
				CancerAbilitySystemComponent->MakeEffectContext());
			}
		}
	}
}
