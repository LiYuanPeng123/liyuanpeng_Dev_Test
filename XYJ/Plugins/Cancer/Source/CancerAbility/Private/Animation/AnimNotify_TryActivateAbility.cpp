
#include "Animation/AnimNotify_TryActivateAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UAnimNotify_TryActivateAbility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		if (AbilityTag.IsValid())
		{
			bool IsActivate = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));
			/*UE_LOG(LogTemp, Log, TEXT("动画通知激活[%s] [%s]"), *AbilityTag.ToString());*/
		}
	}
}

FString UAnimNotify_TryActivateAbility::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}
