#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PlayExecutionSequence.generated.h"

class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;

UCLASS()
class CANCERABILITY_API UAbilityTask_PlayExecutionSequence : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnSequenceStarted;

	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnSequenceFinished;

	UPROPERTY(BlueprintAssignable)
	FGenericGameplayTaskDelegate OnSequenceFailed;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
	static UAbilityTask_PlayExecutionSequence* CreateTask(UGameplayAbility* OwningAbility, ULevelSequence* Sequence, AActor* OriginActor, TMap<FName, AActor*> Bindings, bool bEnableDebug = false);

	virtual void Activate() override;

protected:
	UPROPERTY()
	TObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY()
	TObjectPtr<AActor> Origin;

	UPROPERTY()
	TMap<FName, TObjectPtr<AActor>> BindingMap;

	UPROPERTY()
	TObjectPtr<ALevelSequenceActor> SequenceActor;

	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;

	UPROPERTY()
	bool bDebug;

	UFUNCTION()
	void HandleSequenceFinished();
};
