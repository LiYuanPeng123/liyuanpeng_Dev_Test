
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "AnimNotify_DiscardWeapon.generated.h"

/**
 * 
 */
UCLASS(DisplayName="DiscardWeapon")
class CANCERABILITY_API UAnimNotify_DiscardWeapon : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=QuickIns)
	FName SocketName;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
};
