
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "AnimNotify_AICombo.generated.h"

class UInputAction;
/**
 * AI连招
 */
UCLASS()
class CANCERABILITY_API UAnimNotify_AICombo : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* InputAction;
};
