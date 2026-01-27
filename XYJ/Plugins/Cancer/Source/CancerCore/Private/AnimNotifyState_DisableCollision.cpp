// 


#include "AnimNotifyState_DisableCollision.h"

#include "Interfaces/CancerCoreSystemInterface.h"

void UAnimNotifyState_DisableCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration,
                                                    const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto Actor = MeshComp->GetOwner())
	{
		if (Actor->Implements<UCancerCoreSystemInterface>())
		{
			ICancerCoreSystemInterface::Execute_DisableCharacterCollision(Actor);
		}
	}
}

void UAnimNotifyState_DisableCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (auto Actor = MeshComp->GetOwner())
	{
		if (Actor->Implements<UCancerCoreSystemInterface>())
		{
			ICancerCoreSystemInterface::Execute_EnableCharacterCollision(Actor);
		}
	}
}

FString UAnimNotifyState_DisableCollision::GetNotifyName_Implementation() const
{
	return TEXT("禁用角色碰撞");
}
