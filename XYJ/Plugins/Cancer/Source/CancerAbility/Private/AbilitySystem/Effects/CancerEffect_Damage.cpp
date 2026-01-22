#include "AbilitySystem/Effects/CancerEffect_Damage.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Effects/CancerEffect_Dead.h"
#include "AbilitySystem/Executions/CancerExecution_Damage.h"
#include "GameplayEffectComponents/AdditionalEffectsGameplayEffectComponent.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"

FName UCancerEffect_Damage::AssetTagsName = "AssetTags";
FName UCancerEffect_Damage::AdditionalEffectsName = "AdditionalEffects";

UCancerEffect_Damage::UCancerEffect_Damage(const FObjectInitializer& ObjectInitializer)
{
	//DurationPolicy = EGameplayEffectDurationType::Instant;

	//设置计算类
	FGameplayEffectExecutionDefinition& DamageExecution = Executions.AddDefaulted_GetRef();
	DamageExecution.CalculationClass = UCancerExecution_Damage::StaticClass();

	//添加组件
	AssetTags = CreateDefaultSubobject<UAssetTagsGameplayEffectComponent>(AssetTagsName);
	GEComponents.Add(AssetTags);
	AdditionalEffects = CreateDefaultSubobject<UAdditionalEffectsGameplayEffectComponent>(AdditionalEffectsName);
	AdditionalEffects->bOnApplicationCopyDataFromOriginalSpec = true;
	GEComponents.Add(AdditionalEffects);
	//添加效果
	AddAdditionalEffect(UCancerEffect_Dead::StaticClass());
}

UAssetTagsGameplayEffectComponent* UCancerEffect_Damage::GetAssetTagsComponent() const
{
	return AssetTags;
}

UAdditionalEffectsGameplayEffectComponent* UCancerEffect_Damage::GetAdditionalEffectsComponent() const
{
	return AdditionalEffects;
}

void UCancerEffect_Damage::AddAssetTags(const FGameplayTag& Tag) const
{
	FInheritedTagContainer MyTags;
	MyTags.Added.AddTagFast(Tag);
	AssetTags->SetAndApplyAssetTagChanges(MyTags);
}

void UCancerEffect_Damage::AddAdditionalEffect(const TSubclassOf<UGameplayEffect>& EffectClass) const
{
	FConditionalGameplayEffect& AdditionalEffect = AdditionalEffects->OnApplicationGameplayEffects.
	                                                                  AddDefaulted_GetRef();
	AdditionalEffect.EffectClass = EffectClass;
}
