#include "AbilitySystem/Tasks/AbilityTask_PlayExecutionSequence.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "Kismet/GameplayStatics.h"

UAbilityTask_PlayExecutionSequence* UAbilityTask_PlayExecutionSequence::CreateTask(UGameplayAbility* OwningAbility, ULevelSequence* Sequence, AActor* OriginActor, TMap<FName, AActor*> Bindings, bool bEnableDebug)
{
	UAbilityTask_PlayExecutionSequence* NewTask = NewAbilityTask<UAbilityTask_PlayExecutionSequence>(OwningAbility);
	NewTask->LevelSequence = Sequence;
	NewTask->Origin = OriginActor;
	NewTask->bDebug = bEnableDebug;
	for (const TPair<FName, AActor*>& Entry : Bindings)
	{
		if (Entry.Key != NAME_None && IsValid(Entry.Value))
		{
			NewTask->BindingMap.Add(Entry.Key, Entry.Value);
		}
	}
	return NewTask;
}

void UAbilityTask_PlayExecutionSequence::Activate()
{
	Super::Activate();

	if (!IsValid(LevelSequence) || !IsValid(Origin))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnSequenceFailed.Broadcast();
		}
		EndTask();
		return;
	}

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	PlaybackSettings.bDisableLookAtInput = true;
	PlaybackSettings.bDisableMovementInput = true;
	PlaybackSettings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceKeepState;

	SequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>();
	if (!IsValid(SequenceActor))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnSequenceFailed.Broadcast();
		}
		EndTask();
		return;
	}

	SequenceActor->bOverrideInstanceData = true;
	SequenceActor->SetActorLocation(Origin->GetActorLocation());
	SequenceActor->SetActorRotation(Origin->GetActorRotation());

	if (UDefaultLevelSequenceInstanceData* InstanceData = Cast<UDefaultLevelSequenceInstanceData>(SequenceActor->DefaultInstanceData))
	{
		InstanceData->TransformOriginActor = SequenceActor;
	}

	SequenceActor->SetSequence(LevelSequence);

	for (const TPair<FName, TObjectPtr<AActor>>& Entry : BindingMap)
	{
		if (IsValid(Entry.Value))
		{
			TArray<AActor*> Actors;
			Actors.Add(Entry.Value.Get());
			SequenceActor->SetBindingByTag(Entry.Key, Actors);
		}
	}

	SequenceActor->PlaybackSettings = PlaybackSettings;
	SequenceActor->GetSequencePlayer()->Initialize(LevelSequence, Origin->GetLevel(), FLevelSequenceCameraSettings(false));

	SequencePlayer = SequenceActor->GetSequencePlayer();
	if (!IsValid(SequencePlayer))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnSequenceFailed.Broadcast();
		}
		EndTask();
		return;
	}

	SequencePlayer->OnFinished.AddDynamic(this, &ThisClass::HandleSequenceFinished);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnSequenceStarted.Broadcast();
	}

	SequencePlayer->Play();
}

void UAbilityTask_PlayExecutionSequence::HandleSequenceFinished()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnSequenceFinished.Broadcast();
	}
	if (IsValid(SequenceActor))
	{
		SequenceActor->Destroy();
	}
	EndTask();
}
