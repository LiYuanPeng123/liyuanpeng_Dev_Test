#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Transition.h"
#include "ComboGraph.generated.h"

class UMoveDefinition;

/**
 * 连招图（数据资产）
 * - StartMove：起始段
 * - Transitions：当前段 → 过渡集合
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UComboGraph : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TObjectPtr<UMoveDefinition>, FCancerTransitionList> Transitions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCancerTransitionList StartTransitions;

	UFUNCTION(BlueprintCallable)
	TArray<FTransition> GetTransitions(UMoveDefinition* Move) const;
};
