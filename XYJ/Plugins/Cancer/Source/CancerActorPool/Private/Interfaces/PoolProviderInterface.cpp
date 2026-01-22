
#include"Interfaces/PoolProviderInterface.h"
#include "CancerActorPoolComponent.h"

UCancerActorPoolComponent* IPoolProviderInterface::GetActorPool_Implementation() const
{
	const AActor* SelfActor = Cast<AActor>(this);
	return IsValid(SelfActor) ? SelfActor->FindComponentByClass<UCancerActorPoolComponent>() : nullptr;	
}
