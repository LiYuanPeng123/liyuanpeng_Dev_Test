#include "Animation/AnimNotifyState_ApplyGameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

void UAnimNotifyState_ApplyGameplayEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                       float TotalDuration,
                                                       const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!GameplayEffectClass)
	{
		return;
	}

	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel,
		                                                             ASC->MakeEffectContext());

		if (SpecHandle.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			if (Spec)
			{
				// 如果设置了有效的持续时间（>0），则尝试覆盖
				if (Duration > 0.0f)
				{
					Spec->SetDuration(Duration, true);
				}

				// 应用效果
				FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);

				// 存储句柄以便后续移除(仅限无限持续时间的GE，如果是HasDuration则让其自然结束)
				if (ActiveHandle.IsValid() && SpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite)
				{
					ActiveEffectHandles.Add(MeshComp, ActiveHandle);
				}
			}
		}
	}
}

void UAnimNotifyState_ApplyGameplayEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                     const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// 移除效果
	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		if (FActiveGameplayEffectHandle* HandlePtr = ActiveEffectHandles.Find(MeshComp))
		{
			if (HandlePtr->IsValid())
			{
				ASC->RemoveActiveGameplayEffect(*HandlePtr);
			}
			ActiveEffectHandles.Remove(MeshComp);
		}
	}
}

void UAnimNotifyState_ApplyGameplayEffect::HandleTimeExpired(USkeletalMeshComponent* MeshComp,
                                                             UAnimSequenceBase* Animation)
{
	Super::HandleTimeExpired(MeshComp, Animation);

	// 时间到期时也移除效果
	if (auto ASC = GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		if (FActiveGameplayEffectHandle* HandlePtr = ActiveEffectHandles.Find(MeshComp))
		{
			if (HandlePtr->IsValid())
			{
				ASC->RemoveActiveGameplayEffect(*HandlePtr);
			}
			ActiveEffectHandles.Remove(MeshComp);
		}
	}
}

FString UAnimNotifyState_ApplyGameplayEffect::GetNotifyName_Implementation() const
{
	if (GameplayEffectClass)
	{
		return FString::Printf(TEXT("施加游戏效果: %s (等级%d)"), *GameplayEffectClass->GetName(), EffectLevel);
	}
	return TEXT("施加游戏效果");
}
