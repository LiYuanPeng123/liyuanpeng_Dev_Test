#include "AbilitySystem/Effects/CancerEffect_Soul.h"
#include "AbilitySystem/Attributes/CancerSoulSet.h"
#include "AbilitySystem/Executions/CancerExecution_Soul.h"
#include "CancerNativeGamePlayTag.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UCancerEffect_Soul::UCancerEffect_Soul(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	Period = 0.1f;
	bExecutePeriodicEffectOnApplication = true;

	// 添加目标与资产标签：恢复气力状态（用于免疫气力伤害与移除查询）
	{
		FInheritedTagContainer MyTags;
		MyTags.Added.AddTagFast(Tag_Combat_State_RestoreSoul);
		UTargetTagsGameplayEffectComponent* TargetTags = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("SoulTargetTags"));
		TargetTags->SetAndApplyTargetTagChanges(MyTags);
		GEComponents.Add(TargetTags);

		UAssetTagsGameplayEffectComponent* AssetTags = CreateDefaultSubobject<UAssetTagsGameplayEffectComponent>(TEXT("SoulAssetTags"));
		AssetTags->SetAndApplyAssetTagChanges(MyTags);
		GEComponents.Add(AssetTags);
	}

	//设置计算类
	FGameplayEffectExecutionDefinition& DamageExecution = Executions.AddDefaulted_GetRef();
	DamageExecution.CalculationClass = UCancerExecution_Soul::StaticClass();
}
