
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RootMotionCurveNotifyState.generated.h"

UENUM()
enum class ECancerRootMotionMode
{
	Add UMETA(DisplayName="添加"),
	Override UMETA(DisplayName="覆盖"),
};

UCLASS(Abstract)
class CANCERANIMATION_API URootMotionCurveNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	// 三维曲线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RootMotion")
	FName CurveX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RootMotion")
	FName CurveY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RootMotion")
	FName CurveZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RootMotion")
	FName RootMotionSourceName;
	// 根运动模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RootMotion")
	ECancerRootMotionMode RootMotionMode = ECancerRootMotionMode::Override;

	// 缩放系数，可调节根运动幅度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RootMotion")
	FVector Scale = FVector(1.f, 1.f, 1.f);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:
	FVector PreviousOffset;
};
