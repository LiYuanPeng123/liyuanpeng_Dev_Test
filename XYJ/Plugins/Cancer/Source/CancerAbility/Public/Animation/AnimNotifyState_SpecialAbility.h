#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/CombatAbility_SpecialAbility.h"
#include "Animation/CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_SpecialAbility.generated.h"

class UCancerSpecialAbility;
class UInputAction;
/**
 * 
 */
UCLASS(DisplayName="特殊派生技能")
class CANCERABILITY_API UAnimNotifyState_SpecialAbility : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced, Category="Special Abilities")
	UCancerSpecialAbility* SpecialAbility;
	UPROPERTY(EditAnywhere, Category="Special Abilities")
	FGameplayTagContainer AbilityTag;
	UPROPERTY(EditAnywhere, Category="Special Abilities")
	FGameplayTag InputTag;
	UPROPERTY(BlueprintReadOnly, Category="Special Abilities")
	FGameplayTag Tag = Tag_Combat_State_SpecialAbilityWindow;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
