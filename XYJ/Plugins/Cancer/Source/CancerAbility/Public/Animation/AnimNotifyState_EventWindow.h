// GShenLin

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/CancerCombatAnimNotifyState.h"
#include "AnimNotifyState_EventWindow.generated.h"

/**
 * 在开始与结束处分别发送事件通知
 */
UCLASS()
class CANCERABILITY_API UAnimNotifyState_EventWindow : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
							 float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						   const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag BeginEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag EndEventTag;
	
	// 用于显示的事件名称
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString EventName;
};
