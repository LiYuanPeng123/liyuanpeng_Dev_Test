#include "AbilitySystem/Tasks/AbilityTask_TargetingTick.h"
#include "TargetingSystem/TargetingSubsystem.h"

UAbilityTask_TargetingTick::UAbilityTask_TargetingTick()
{
	bTickingTask = true;
}

UAbilityTask_TargetingTick* UAbilityTask_TargetingTick::TargetingPresetTick(UGameplayAbility* OwningAbility, UTargetingPreset* InPreset)
{
	UAbilityTask_TargetingTick* NewTask = NewAbilityTask<UAbilityTask_TargetingTick>(OwningAbility);
	NewTask->TargetingPreset = InPreset;
	NewTask->StartLocation.SourceAbility = OwningAbility;
	NewTask->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	NewTask->StartLocation.SourceActor = OwningAbility->GetAvatarActorFromActorInfo();
	return NewTask;
}

void UAbilityTask_TargetingTick::Activate()
{
	Super::Activate();
}

void UAbilityTask_TargetingTick::TickTask(float DeltaTime)
{
	if (!TargetingPreset)
	{
		EndTask();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		EndTask();
		return;
	}

	UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	if (!TargetingSubsystem)
	{
		EndTask();
		return;
	}

	FTargetingSourceContext SourceContext;
	SourceContext.SourceActor = StartLocation.SourceActor.Get();
	SourceContext.SourceLocation = SourceContext.SourceActor
		                               ? SourceContext.SourceActor->GetActorLocation()
		                               : FVector::ZeroVector;

	FTargetingRequestHandle TargetingHandle = UTargetingSubsystem::MakeTargetRequestHandle(
		TargetingPreset, SourceContext);

	FTargetingRequestDelegate Delegate = FTargetingRequestDelegate::CreateWeakLambda(
		this, [this](FTargetingRequestHandle TargetingHandle)
		{
			OnTargetUpdated.Broadcast(TargetingHandle);
		});

	TargetingSubsystem->ExecuteTargetingRequestWithHandle(TargetingHandle, Delegate);
}
