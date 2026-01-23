
#include "Action_AddActorPoolComponent.h"

#include "CancerActorPoolComponent.h"

void UAction_AddActorPoolComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GatherSoftReferences(OutPaths);
}

void UAction_AddActorPoolComponent::PostInitComponent_Implementation()
{
	UCancerActorPoolComponent* ActorPoolComponent = Cast<UCancerActorPoolComponent>(GetCreateComponent());
	check(ActorPoolComponent != nullptr);
	ActorPoolComponent->SetPoolableActors(PoolableActors);
}
