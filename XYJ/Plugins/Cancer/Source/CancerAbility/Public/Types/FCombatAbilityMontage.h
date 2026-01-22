#pragma once
#include "GameplayTagContainer.h"
#include "FCombatAbilityMontage.generated.h"

USTRUCT(BlueprintType)
struct FCombatAbilityMontage : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite, DisplayName="蒙太奇")
	UAnimMontage* Montage = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, DisplayName="蒙太奇标签")
	FGameplayTag MontageTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, DisplayName="蒙太奇描述")
	FName MontageDisplayName = "";
};
