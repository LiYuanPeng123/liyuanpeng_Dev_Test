#include "Animation/AnimNotifyState_ApplyIronBody.h"

#include "CancerAbilityFunctionLibrary.h"
#include "AbilitySystem/Effects/CancerEffect_IronBody.h"
#include "AbilitySystem/Attributes/CancerCombatSet.h"
#include "Components/CancerCombatAttributeComponent.h"

UAnimNotifyState_ApplyIronBody::UAnimNotifyState_ApplyIronBody()
{
	IronBodyLevel = 0;
#if WITH_EDITORONLY_DATA
	// Ninja Bear Green! :D
	NotifyColor = FColor(255, 0, 255);
#endif
}

void UAnimNotifyState_ApplyIronBody::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);


	if (!UCancerAbilityFunctionLibrary::K2_ApplyIronBody(
		MeshComp->GetOwner(), IronBodyLevel,
		UCancerEffect_IronBody::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("<--霸体应用失败-->"))
	}
}

void UAnimNotifyState_ApplyIronBody::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (auto ASC = UCancerAbilityFunctionLibrary::GetCancerAbilitySystemComponent(MeshComp->GetOwner()))
	{
		if (UCancerCombatAttributeComponent* CombatComponent =
			UCancerAbilityFunctionLibrary::GetCancerCombatAttributeComponent(MeshComp->GetOwner()))
		{
			const float BaseIronBody = CombatComponent->GetBaseIronBody();
			ASC->ApplyModToAttribute(UCancerCombatSet::GetIronBodyAttribute(), EGameplayModOp::Override, BaseIronBody);
		}
	}
}

FString UAnimNotifyState_ApplyIronBody::GetNotifyName_Implementation() const
{
	return TEXT("霸体");
}
