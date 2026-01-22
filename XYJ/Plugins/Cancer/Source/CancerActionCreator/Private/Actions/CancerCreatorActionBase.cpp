
#include"Actions/CancerCreatorActionBase.h"



UCancerCreatorActionBase::UCancerCreatorActionBase()
{
}

void UCancerCreatorActionBase::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
}


void UCancerCreatorActionBase::PostExecuteAction_Implementation(AActor* InActor)
{
}

void UCancerCreatorActionBase::PreExecuteAction_Implementation(AActor* InActor)
{
	CreatActor = InActor;
}

void UCancerCreatorActionBase::PreInitComponent_Implementation()
{
}

void UCancerCreatorActionBase::PostInitComponent_Implementation()
{
}

void UCancerCreatorActionBase::PostDestroyAction_Implementation(AActor* InActor)
{
}

void UCancerCreatorActionBase::PreDestroyAction_Implementation(AActor* InActor)
{
}

void UCancerCreatorActionBase::FinalizeAfterComponent_Implementation()
{
}

AActor* UCancerCreatorActionBase::GetCreatActor() const
{
	return CreatActor.Get();
}
