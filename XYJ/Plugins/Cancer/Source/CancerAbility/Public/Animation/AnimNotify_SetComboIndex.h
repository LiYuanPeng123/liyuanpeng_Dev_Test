#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "AnimNotify_SetComboIndex.generated.h"

/**
 * 设置连招段数
 */
UCLASS()
class CANCERABILITY_API UAnimNotify_SetComboIndex : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_SetComboIndex();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combo")
	int32 ComboIndex = 0;
	virtual FString GetNotifyName_Implementation() const override;
};
