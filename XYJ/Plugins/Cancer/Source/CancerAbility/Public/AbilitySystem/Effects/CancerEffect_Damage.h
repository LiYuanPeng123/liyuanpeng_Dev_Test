
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_Damage.generated.h"


class UAssetTagsGameplayEffectComponent;
class UAdditionalEffectsGameplayEffectComponent;

/**
 * 伤害
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_Damage : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** 该组件所使用的名称，若您需要对其进行修改，请在此处指定。*/
	static FName AssetTagsName;
	/** 该组件所使用的名称，若您需要对其进行修改，请在此处指定。*/
	static FName AdditionalEffectsName;

	UCancerEffect_Damage(const FObjectInitializer& ObjectInitializer);

	UAssetTagsGameplayEffectComponent* GetAssetTagsComponent() const;

	UAdditionalEffectsGameplayEffectComponent* GetAdditionalEffectsComponent() const;

protected:

	/** Adds a Gameplay Tag to the internal Asset Tag component. */
	void AddAssetTags(const FGameplayTag& Tag) const;

	/** Adds a Gameplay Effect to apply with this one. */
	void AddAdditionalEffect(const TSubclassOf<UGameplayEffect>& EffectClass) const;

private:
	
	/** Component containing tags available to this asset. */
	UPROPERTY()
	TObjectPtr<UAssetTagsGameplayEffectComponent> AssetTags;

	/** Adds other effects along with this one. */
	UPROPERTY()
	TObjectPtr<UAdditionalEffectsGameplayEffectComponent> AdditionalEffects;
	
};
