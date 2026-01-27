
#include "AbilitySystem/Abilities/CombatAbility_DamageFeedback.h"
#include "AbilitySystemComponent.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"
#include "CancertGlobalAbilitySystem.h"
#include "Components/CancerQuickBarComponent.h"
#include "GameFramework/CancerDamageType.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCombatAbility_DamageFeedback::UCombatAbility_DamageFeedback(const FObjectInitializer& ObjectInitializer)
{
}

void UCombatAbility_DamageFeedback::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData)
	{
		HandleEventReceived(*TriggerEventData);
	}
}

void UCombatAbility_DamageFeedback::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	Super::HandleEventReceived_Implementation(Payload);
	if (Payload.EventTag != Tag_Combat_Event_AbilityTrigger_DamageFeedback) return;
	
	// 取伤害类型
	const UCancerDamageType* DamageType = Cast<UCancerDamageType>(Payload.OptionalObject);
	if (!IsValid(DamageType) || !DamageType->DamageParameter.HitInfo.DamageTag.IsValid())
	{
		K2_EndAbility();
		return;
	}
	if (DamageType->DamageParameter.DamageFeedback.DamageInfo.HasTag(Tag_Combat_DamageFeedBack_Invulnerable))
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageFeedback: Invulnerable"));
	}
	
	
	if (DamageType->DamageParameter.DamageFeedback.DamageInfo.HasTag(Tag_Combat_DamageFeedBack_PerfectBlock))
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageFeedback: PerfectBlock"));
		
		// 攻击者被击退
		if (UAbilitySystemComponent* ASC =  GetAbilitySystemComponentFromActorInfo())
		{
			
			ASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_Stagger, &Payload);
			
			// 攻击方虚弱 可能是由于完美弹反
			if (ASC->HasMatchingGameplayTag(Tag_Combat_State_Weak))
			{
				UE_LOG(LogTemp, Warning, TEXT("DamageFeedback: Attacker Weak! "));
				// 丢弃武器
				if (AActor* OwnerActor = ASC->GetOwnerActor())
				{
					UCancerQuickBarComponent* CancerQuickBarComponent =  UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(OwnerActor);
					CancerQuickBarComponent->GetQuickBarIns(QuickBar::QuickBar_Weapon)->DiscardItemInSlot(OwnerActor->GetActorTransform());
				}
			}
		}
	}
	
	if (DamageType->DamageParameter.HitInfo.TriggerTargetAbilityTag == Tag_Combat_Ability_Replaceable_Hit_BeiDaJiXiHun)
	{
		UE_LOG(LogTemp, Warning, TEXT("XiHun Success Trigger"));
		if (UAbilitySystemComponent* ASC =  GetAbilitySystemComponentFromActorInfo())
		{
			// 这里需要确认一下是不是打到的是玩家 也可能释放抓取技 打到了别人
			if (AActor* OwnerActor = ASC->GetOwnerActor())
			{
				ACharacter* PlayerCharacter  = UGameplayStatics::GetPlayerCharacter(OwnerActor,0);
				AActor* PlayerActor = Cast<AActor>(PlayerCharacter);
				if (DamageType->DamageParameter.HitInfo.HitResult.GetActor() == PlayerActor)
				{
					ASC->HandleGameplayEvent(Tag_Combat_Event_AbilityTrigger_DaJi_XiHunSuccess, &Payload);
				}
			}
		}
	}
	
	if (DamageType->DamageParameter.DamageFeedback.DamageInfo.HasTag(Tag_Combat_DamageFeedBack_Block))
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageFeedback: Block"));
	}
	
	if (DamageType->DamageParameter.DamageFeedback.DamageInfo.HasTag(Tag_Combat_DamageFeedBack_Kill))
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageFeedback: Kill"));
	}
	
	if (DamageType->DamageParameter.DamageFeedback.DamageInfo.HasTag(Tag_Combat_DamageFeedBack_NoSoul))
	{
		// 受击方被打到虚弱状态
		UE_LOG(LogTemp, Warning, TEXT("DamageFeedback: NoSoul"));
	}
	
	K2_EndAbility();
}

