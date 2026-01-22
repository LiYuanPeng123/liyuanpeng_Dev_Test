
#pragma once

#include "CoreMinimal.h"
#include "Providers/CancerCombatDirectionalAnimationProvider.h"
#include "AnimationProvider_DirectionalEvade.generated.h"


UCLASS(meta = (DisplayName = "Directional Evades"))
class CANCERABILITY_API UAnimationProvider_DirectionalEvade : public UCancerCombatDirectionalAnimationProvider
{
	GENERATED_BODY()
public:

	UAnimationProvider_DirectionalEvade();
	
protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Directional Evade")
	float Offset;


	// -- Begin Direction Animation Provider implementation
	virtual bool CalculateAnimationAngle_Implementation(const UCancerGameplayAbility* CombatAbility, float& OutAngle) const override;
	// -- End Direction Animation Provider implementati
};
