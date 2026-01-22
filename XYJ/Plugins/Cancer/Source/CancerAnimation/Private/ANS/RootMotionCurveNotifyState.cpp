// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS/RootMotionCurveNotifyState.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void URootMotionCurveNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character) return;

	PreviousOffset = FVector::ZeroVector;

	// 覆盖模式时先清除已有 RootMotion
	if (RootMotionMode == ECancerRootMotionMode::Override)
	{
		Character->GetCharacterMovement()->RemoveRootMotionSource(RootMotionSourceName);
	}
	// 创建 RMS
	TSharedPtr<FRootMotionSource_ConstantForce> RMS = MakeShared<FRootMotionSource_ConstantForce>();
	RMS->InstanceName =RootMotionSourceName;
	RMS->AccumulateMode = (RootMotionMode == ECancerRootMotionMode::Override) ? 
		ERootMotionAccumulateMode::Override : ERootMotionAccumulateMode::Additive;
	RMS->Force = FVector::ZeroVector; // 初始为 0
	RMS->Duration = TotalDuration;
	RMS->Priority = 1000;

	Character->GetCharacterMovement()->ApplyRootMotionSource(RMS);
}

void URootMotionCurveNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character) return;

	// 结束时移除 RootMotionSource
	Character->GetCharacterMovement()->RemoveRootMotionSource(RootMotionSourceName);
	PreviousOffset = FVector::ZeroVector;
}

void URootMotionCurveNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || FrameDeltaTime <= 0.f) return;
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character) return;

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance) return;

	// 获取动画实例当前帧曲线值
	float ValueX = CurveX.IsNone() ? 0.f : AnimInstance->GetCurveValue(CurveX);
	float ValueY = CurveY.IsNone() ? 0.f : AnimInstance->GetCurveValue(CurveY);
	float ValueZ = CurveZ.IsNone() ? 0.f : AnimInstance->GetCurveValue(CurveZ);

	FVector CurveDelta(ValueX * Scale.X, ValueY * Scale.Y, ValueZ * Scale.Z);
	FVector DeltaMovement = CurveDelta * FrameDeltaTime;

	TSharedPtr<FRootMotionSource_ConstantForce> RMS =
		StaticCastSharedPtr<FRootMotionSource_ConstantForce>(
			Character->GetCharacterMovement()->GetRootMotionSource(RootMotionSourceName)
		);

	if (RMS.IsValid())
	{
		RMS->Force = DeltaMovement / FrameDeltaTime;
	}
}
