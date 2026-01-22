
#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS(DisplayName="发送游戏事件(ASC)")
class CANCERABILITY_API UAnimNotify_SendGameplayEvent : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation System")
	FGameplayTag EventTag;
};
