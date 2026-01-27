#include "Animation/AnimNotify_ApplyEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UAnimNotify_ApplyEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel,
		                                                             ASC->MakeEffectContext());

		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				// 如果设置了有效的持续时间（>0），则尝试覆盖持续时间
				// 注意：对于Infinite或Instant类型的GE，底层GAS可能会忽略此持续时间，这取决于GE的配置
				if (Duration > 0.0f)
				{
					Spec->SetDuration(Duration, true);
				}

				ASC->ApplyGameplayEffectSpecToSelf(*Spec);
			}
		}
	}
}

FString UAnimNotify_ApplyEffect::GetNotifyName_Implementation() const
{
	return TEXT("ApplyEffect") + DisplayName;
}
