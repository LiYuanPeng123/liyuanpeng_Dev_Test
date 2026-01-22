#pragma once
#include "GameplayTagContainer.h"
#include "GameplayAbilities/Public/ScalableFloat.h"
#include "FCombatAbilitySet.generated.h"

class UCancerGameplayAbility;


UENUM(BlueprintType)
enum class ECancerAbilityGroup : uint8
{
	Exclusive_Replaceable = 0,
	Exclusive_Blocking = 1,
	Independent = 2,
	BlockingAll = 3,
	MAX UMETA(Hidden)
};

/*USTRUCT(BlueprintType)
struct FCombatAbilitySet : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cancer|Ability Activation")
	ECancerAbilityGroup ActivationGroup{ECancerAbilityGroup::Exclusive_Replaceable};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UCancerGameplayAbility> GameplayAbility = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AbilityTag;
};*/


USTRUCT(BlueprintType)
struct FCancerAbilityPriority : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cancer|Ability Activation")
	ECancerAbilityGroup ActivationGroup{ECancerAbilityGroup::Exclusive_Replaceable};
};

USTRUCT(BlueprintType)
struct FCancerAbilityDef : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Def")
	int32 DefaultLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly ,Category = "Def",meta=(Categories="InputTag"),DisplayName="输入标签")
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Def",meta=(Categories="Ability"),DisplayName="技能标签")
	FGameplayTagContainer AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Def", meta=(Categories="Combat.Event"), DisplayName="触发标签")
	FGameplayTag TriggerTag;
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Def")
	FGameplayTagContainer OwnedTags;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Def")
	FScalableFloat CooldownDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Def")
	FGameplayTagContainer CooldownTags;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Def")
	FScalableFloat Cost;
};


USTRUCT(BlueprintType)
struct FCancerAbilityDataSet : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cancer")
	TSubclassOf<UCancerGameplayAbility> Ability = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Cancer")
	FCancerAbilityPriority AbilityPriority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Cancer")
	FCancerAbilityDef AbilityDef;
};



USTRUCT(BlueprintType)
struct FComboAttributeSet 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FGameplayTag,FScalableFloat> AttributeMap;
};