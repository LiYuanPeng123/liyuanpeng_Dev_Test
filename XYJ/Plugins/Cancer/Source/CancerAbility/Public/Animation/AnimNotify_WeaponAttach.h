
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "Equipment/EquipmentInstance.h"
#include "AnimNotify_WeaponAttach.generated.h"

/**
 * 
 */
UCLASS(DisplayName="AttachWeapon")
class CANCERABILITY_API UAnimNotify_WeaponAttach : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere,category=Input)
	EEquipSocket InEquipSocket;
};
