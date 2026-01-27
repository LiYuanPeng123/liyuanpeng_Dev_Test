#pragma once

#include "CoreMinimal.h"
#include "Animation/CancerCombatAnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_TargetSectorDetection.generated.h"

/**
 * 扇形目标检测通知状态
 * 在指定扇形区域内检测目标
 */
UCLASS()
class CANCERABILITY_API UAnimNotifyState_TargetSectorDetection : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_TargetSectorDetection();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection", DisplayName="检测通道")
	TArray<TEnumAsByte<ECollisionChannel>> DetectionChannels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection", DisplayName="检测角度")
	float Angle = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection", DisplayName="检测半径")
	float Radius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection", DisplayName="检测高度")
	float Height = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection", DisplayName="起始点偏移")
	FVector DetectionOffset = FVector::ZeroVector;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug", DisplayName="显示调试绘制")
	bool bDebug = true;

private:
	/** 获取检测原点和朝向 */
	void GetSearchInfo(USkeletalMeshComponent* MeshComp, FVector& OutLocation, FVector& OutForward) const;
	
	/** 绘制调试形状 */
	void DrawDebug(USkeletalMeshComponent* MeshComp);

	/** 当前已添加标签的目标集合，用于 NotifyEnd 时清理（如果需要） */
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AffectedActors;
};
