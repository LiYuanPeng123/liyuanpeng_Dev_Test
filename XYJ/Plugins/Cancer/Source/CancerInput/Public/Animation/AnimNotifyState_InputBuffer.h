
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_InputBuffer.generated.h"


UCLASS(Abstract)
class CANCERINPUT_API UAnimNotifyState_InputBuffer : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:

	UAnimNotifyState_InputBuffer();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	FGameplayTag BufferChannelTag;
	
	virtual void CloseBuffer(AActor* BufferOwner);

#if WITH_EDITOR
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override;
#endif
	
};
