
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CancerEffect_Dead.generated.h"

class URemoveOtherGameplayEffectComponent;
/**
 * 死亡
 */
UCLASS()
class CANCERABILITY_API UCancerEffect_Dead : public UGameplayEffect
{
	GENERATED_BODY()
public:

	UCancerEffect_Dead();

protected:

	void RemoveGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass) const;
	
private:

	/** Removes other relevant effects. */
	UPROPERTY()
	TObjectPtr<URemoveOtherGameplayEffectComponent> RemovedEffects;	
};
