
#pragma once
#include "CoreMinimal.h"
#include "CancerAbilityCost.h"
#include "CancerAbilityCost_GameplayEffect.generated.h"


UENUM(BlueprintType)
enum class ECancerCostCheckType:uint8
{
	Equal UMETA(DisplayName = "等于"),
	NotEqual  UMETA(DisplayName = "不等于"),
	Greater  UMETA(DisplayName = "大于"),
	GreaterOrEqual  UMETA(DisplayName = "大于等于"),
	Less  UMETA(DisplayName = "小于"),
	LessOrEqual  UMETA(DisplayName = "小于等于")
};

USTRUCT(BlueprintType)
struct FCancerCostCheck
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = Costs)
	bool MustTarget = false;
	UPROPERTY(EditAnywhere, Category=Costs, DisplayName="(源)属性")
	FGameplayAttribute CheckAttribute;
	UPROPERTY(EditAnywhere, Category=Costs, DisplayName="(目标)属性",
		meta=(EditCondition = "MustTarget", EditConditionHides))
	FGameplayAttribute CheckAttributeBase;
	UPROPERTY(EditAnywhere, Category=Costs, DisplayName="修饰符")
	ECancerCostCheckType CostCheckType{ECancerCostCheckType::Greater};
	UPROPERTY(EditAnywhere, Category=Costs, DisplayName="值",
		meta=(ClampMin = 0, EditCondition = "MustTarget == false", EditConditionHides))
	float Value = 0.0f;
	UPROPERTY(EditAnywhere, Category=Costs, DisplayName="百分比值",
		meta=(ClampMin = 0, ClampMax = 1, EditCondition = "MustTarget", EditConditionHides))
	float PercentValue = 0.0f;
};

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UCancerAbilityCost_GameplayEffect : public UCancerAbilityCost
{
	GENERATED_BODY()

public:
	virtual bool CheckCost(const UCancerGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCost(const UCancerGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Costs)
	TSubclassOf<class UGameplayEffect> CostGameplayEffectClass;
	UPROPERTY(EditDefaultsOnly, Category = Costs)
	int32 CostLevel = 1;
	/*UPROPERTY(EditDefaultsOnly, Category = Costs, DisplayName="自定义消耗")
	bool CustomCost = false;
	UPROPERTY(EditAnywhere, Category = Costs, DisplayName = "", meta=(EditCondition = "CustomCost", EditConditionHides))
	TArray<FGameplayTag> InjectionTags;*/
	
	UPROPERTY(EditDefaultsOnly, Category = Costs, DisplayName="自定义消耗检测")
	bool CustomCostCheck = false;
	UPROPERTY(EditAnywhere, Category = Costs, DisplayName = "", meta=(EditCondition = "CustomCostCheck", EditConditionHides))
	TArray<FCancerCostCheck> CostChecks;
};
