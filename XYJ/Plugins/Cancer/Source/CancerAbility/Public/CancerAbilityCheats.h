#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "CancerAbilityCheats.generated.h"

class UCancerCombatAttributeComponent;
class UCancerAbilitySystemComponent;
/**
 *  
 */
UCLASS()
class CANCERABILITY_API UCancerAbilityCheats : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UCancerAbilityCheats();

	UFUNCTION(Exec, BlueprintCallable)
	void DamageSelfDestruct();


	UCancerCombatAttributeComponent* GetCombatAttributeComponent();
};
