#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"
#include "AnimNotifyState_DisplacementWarp.generated.h"

/**
 * 位移修饰器通知状态（DisplacementWarp）
 */
UCLASS()
class CANCERABILITY_API UAnimNotifyState_DisplacementWarp : public UAnimNotifyState_MotionWarping
{
	GENERATED_BODY()

public:
	UAnimNotifyState_DisplacementWarp(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                        float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;


	void Debug(USkeletalMeshComponent* MeshComp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="角色自动吸附", DisplayName="bAutoAdsorption")
	bool bAutoAdsorption = true;
	UPROPERTY(EditAnywhere, DisplayName="自动吸附检测通道(为空使用项目设置中的检测通道)")
	TArray<TEnumAsByte<ECollisionChannel>> AutoAdsorptionChannels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="自动吸附", DisplayName="bDebug")
	bool bDebug = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="自动吸附", DisplayName="DebugDuration"
		, meta=(EditCondition="bDebug", EditConditionHides))
	float DebugDuration = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="自动吸附", DisplayName="自动吸附角度")
	float Angle = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="自动吸附", DisplayName="自动吸附半径")
	float Radius = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="自动吸附", DisplayName="自动吸附高度")
	float Height = 150.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="WarpName")
	FName WarpTargetName = FName(TEXT("AttackTarget"));

	// 触发修正的检测距离（<=0始终修正）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="触发修正检测距离(cm)")
	float CorrectionTriggerDistance = 800.f;

	// 无目标时的位移距离（<=0不生效）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="无目标位移距离")
	float NoTargetDistance = -1.f;

	// 有目标时的最大位移
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="最大位移距离(有目标)")
	float MaxDisplacementWithTarget = 300.f;

	// 直接位移覆盖距离（2D，>0覆盖所有距离）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="覆盖位移距离(2D)")
	float DirectDistanceOverride = 0.f;

	// 末端位置微调偏移
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="额外偏移（用于微调）")
	FVector FinalOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="忽略Z轴")
	bool bIgnoreZAxis = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DisplacementWarp", DisplayName="扭曲平移")
	bool bWarpTranslation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DisplacementWarp", DisplayName="扭曲旋转")
	bool bWarpRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DisplacementWarp", DisplayName="保留原始位移节奏（按原动画每帧比例缩放）")
	bool bPreserveOriginalRhythm = true;

	virtual URootMotionModifier* AddRootMotionModifier_Implementation(UMotionWarpingComponent* MotionWarpingComp,
	                                                                  const UAnimSequenceBase* Animation,
	                                                                  float StartTime, float EndTime) const override;
};
