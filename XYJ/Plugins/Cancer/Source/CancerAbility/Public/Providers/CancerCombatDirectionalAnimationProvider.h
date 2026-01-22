
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimationProvider.h"
#include "Types/FCombatAnimationDirection.h"
#include "CancerCombatDirectionalAnimationProvider.generated.h"


UCLASS(Abstract)
class CANCERABILITY_API UCancerCombatDirectionalAnimationProvider : public UCancerCombatAnimationProvider
{
	GENERATED_BODY()
public:

	UCancerCombatDirectionalAnimationProvider();
	
	// -- Begin Animation Provider implementation
	virtual FName GetSectionName_Implementation(UCancerGameplayAbility* CombatAbility) const override;
	// -- End Animation Provider implementation

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Provider", meta = (InlineEditConditionToggle))
	bool bUseContextDirections;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Provider", meta = (EditCondition = "bUseContextDirections", TitleProperty = "{SectionName} {Range}"))
	TArray<FCombatAnimationDirection> AnimationDirections;


	UFUNCTION(BlueprintNativeEvent, Category = "Animation Provider")
	bool CalculateAnimationAngle(const UCancerGameplayAbility* CombatAbility, float& OutAngle) const;

private:
	
	/** Helper method to initialize an array with default directions, during construction. */
	static void InitializeDirections(TArray<FCombatAnimationDirection>& DirectionsArray);
	

	
};
