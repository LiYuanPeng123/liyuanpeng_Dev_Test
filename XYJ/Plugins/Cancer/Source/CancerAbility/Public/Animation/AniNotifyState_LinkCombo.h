#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AniNotifyState_LinkCombo.generated.h"

class ULinkComboProvider;
/**
 * 派生招式窗口器
 */
UCLASS(DisplayName="连招派生招式")
class CANCERABILITY_API UAniNotifyState_LinkCombo : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	UAniNotifyState_LinkCombo();

	// -- Begin Notify State implementation
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;
	// -- End Notify State implementation
	virtual void HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Data")
	ULinkComboProvider* LinkComboProvider;
};
