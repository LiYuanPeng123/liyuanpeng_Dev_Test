#include "CancerLinkedAnimInstance.h"
#include "CancerAnimInstance.h"
#include "AnimDistanceMatchingLibrary.h"
#include "GameFramework/Character.h"
#include "ChooserFunctionLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "AnimCharacterMovementLibrary.h"
#include "Components/CancerMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimationAsset.h"

UE_DISABLE_OPTIMIZATION

// 将动画节点引用转换为序列播放器引用
FSequencePlayerReference ConvertToSequencePlayer(const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequencePlayerReference SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	return EAnimNodeReferenceConversionResult::Succeeded == ConversionResult
		       ? SequencePlayer
		       : FSequencePlayerReference();
}

// 将动画节点引用转换为序列求值器引用
FSequenceEvaluatorReference ConvertToSequenceEvaluator(const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);

	return EAnimNodeReferenceConversionResult::Succeeded == ConversionResult
		       ? SequenceEvaluator
		       : FSequenceEvaluatorReference();
}

/**
 * 角色旋转匹配函数：
 * 根据动画和运动数据计算平滑的旋转过渡
 * @param RotationAlpha - 旋转混合系数(0-1)，控制动画旋转的权重
 * @param DeltaTime - 帧时间差，用于平滑插值计算
 * @param InterpSpeed - 插值速度，控制旋转过渡的平滑程度
 * @param LocomotionData - 输入的运动数据(包含加速度、速度等)
 * @param OutCustomRotationData - 输出的自定义旋转数据(包含最终旋转角度)
 * @param OutCurrentAnimData - 输入输出的当前动画数据(包含目标角度、加速度方向等)
 */
void CustomRotationMatching(float RotationAlpha, float DeltaTime, float InterpSpeed,
                            const FLocomotionData& LocomotionData, FCustomRotationData& OutCustomRotationData,
                            FCurrentAnimData& OutCurrentAnimData)
{
	// 计算动画期望旋转：根据混合系数缩放目标角度，因为动画只有L90/L180/R90/R180，而目标角度并不是这四个固定角度
	const float AnimDesiredRotation = FRotator::NormalizeAxis(RotationAlpha * OutCurrentAnimData.TargetAngle);
	// 实时获取当前加速度的方向
	const float TargetAccelerationDirection = LocomotionData.Movement.Acceleration.GetSafeNormal2D().Rotation().Yaw;

	// 平滑插值到加速度方向，避免方向突变，提供自然的旋转过渡
	OutCurrentAnimData.CurrentAccelerationDirection = FRotator::NormalizeAxis(
		FMath::RInterpTo(
			FRotator(0.f, OutCurrentAnimData.CurrentAccelerationDirection, 0.f),
			FRotator(0.f, TargetAccelerationDirection, 0.f),
			DeltaTime, InterpSpeed
		).Yaw
	);

	// 计算加速度差异：当前加速度方向与进入动画时的加速度方向差异
	const float AccelerationDifference = FRotator::NormalizeAxis(
		OutCurrentAnimData.CurrentAccelerationDirection - OutCurrentAnimData.EntryAccelerationAngle);
	// 计算调整后的期望旋转：将加速度变化的影响应用到动画期望旋转上，同时缩放这个差异变化值
	const float DesiredRotationChanged = FRotator::NormalizeAxis(
		AnimDesiredRotation + AccelerationDifference * RotationAlpha);

	// 输出最终自定义旋转：将计算的旋转值结果加到当前的组件旋转值上
	OutCustomRotationData.CustomRotationYaw = FRotator::NormalizeAxis(
		DesiredRotationChanged + OutCurrentAnimData.EntryRotationYaw);
}

UCancerLinkedAnimInstance::UCancerLinkedAnimInstance()
{
	//If true, linked instances will use the main instance's montage data.
	//(i.e. playing a montage on a main instance will play it on the linked layer too.)
	bUseMainInstanceMontageEvaluationData = true;
}

void UCancerLinkedAnimInstance::PostLoad()
{
	Super::PostLoad();
}

void UCancerLinkedAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UCancerLinkedAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Parent = Cast<UCancerAnimInstance>(GetSkelMeshComponent()->GetAnimInstance());
	Character = Cast<ACharacter>(GetOwningActor());
#if WITH_EDITOR
	const auto* World{GetWorld()};

	if (IsValid(World) && !World->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (!Parent.IsValid())
		{
			Parent = GetMutableDefault<UCancerAnimInstance>();
		}

		if (!IsValid(Character))
		{
			Character = GetMutableDefault<ACharacter>();
		}
	}
#endif
}

FAnimInstanceProxy* UCancerLinkedAnimInstance::CreateAnimInstanceProxy()
{
	return Super::CreateAnimInstanceProxy();
}

void UCancerLinkedAnimInstance::Setup_IdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);

	IdleAnimData.AnimSequence = GetAnimSequence(this
	                                            , IdleAnimChooserTable);
	if (!IdleAnimData.AnimSequence)
	{
		return;
	}
	USequencePlayerLibrary::SetSequence(SequencePlayer, IdleAnimData.AnimSequence);

	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EHold;
}

void UCancerLinkedAnimInstance::Setup_StartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	SetUpAnim(StartAnimData);

	StartAnimData.AnimSequence = GetAnimSequence(this
	                                             , StartAnimChooserTable);
	if (!StartAnimData.AnimSequence)
	{
		return;
	}
	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, StartAnimData.AnimSequence);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);

	/*GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EAnimRotation;
	if (ECancerDirection_4::Forward == GetParent()->LocomotionData.Rotation.VelocityDirection)
	{
		if (ECancerDirection_4::Forward == GetParent()->LocomotionData.Rotation.AccelerationDirection)
		{
			GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EInterpolation;
		}
	}*/

	StartAnimData.TargetAngle = GetParent()->LocomotionData.Rotation.LocalAccelerationAngle;
	StartAnimData.CurrentAccelerationDirection = GetParent()->LocomotionData.Movement.Acceleration.GetSafeNormal2D().
	                                                          Rotation().Yaw;
	StartAnimData.EntryAccelerationAngle = StartAnimData.CurrentAccelerationDirection;

	StartAnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;

	// 重置播放速率和步幅适配的Alpha数据
	StartAnimData.FirstStepAlpha = 0.f;
	StartAnimData.StrideWarpingAlpha = 0.f;
}

void UCancerLinkedAnimInstance::Update_StartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	const float DeltaTime = Context.GetContext()->GetDeltaTime();
	const float CurrentTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);
	FAnimExtractContext ExtractCtx(static_cast<double>(CurrentTime));

	/*auto AnimSequence = GetAnimSequence(this
	                                             , StartAnimChooserTable);*/
	if (!StartAnimData.AnimSequence)
	{
		return;
	}
	/*if (StartAnimData.AnimSequence != AnimSequence)
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SequenceEvaluator, StartAnimData.AnimSequence);*/

	const float RotationAlpha = StartAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, ExtractCtx);


	// 步幅混合权重计算：小于0.1s则输出权重为0，大于0.4s则输出权重为1
	StartAnimData.FirstStepAlpha = FMath::GetMappedRangeValueClamped(
		FVector2f(0.1f, 0.4f), // 输入范围：动画时间0.1s~0.4s
		FVector2f(0.f, 1.f), // 输出范围：权重0~1
		CurrentTime // 当前动画时间
	);
	StartAnimData.StrideWarpingAlpha = StartAnimData.FirstStepAlpha;

	FVector2D PlayRateClamped = PlayRateDefault;
	// 距离检查：如果累积距离小于2个单位，就扩大最大播放速率
	if (StartAnimData.AnimSequence->EvaluateCurveData(GetParent()->LocomotionConfig.Distance, ExtractCtx) <
		2.f)
	{
		PlayRateClamped.Y = 2.f;
	}
	// 根据步幅权重插值最终播放速率
	const FVector2D PlayRateFinal = FMath::Lerp(FVector2D(0.5f, 1.5f), PlayRateClamped, StartAnimData.FirstStepAlpha);

	UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(
		Context, SequenceEvaluator,
		GetParent()->LocomotionData.Movement.DisplacementSinceLastUpdate,
		GetParent()->LocomotionConfig.Distance,
		PlayRateFinal
	);
	CustomRotationMatching(RotationAlpha, DeltaTime, 10.f, GetParent()->LocomotionData,
	                       GetParentMovement()->CustomRotationData, StartAnimData);
}

void UCancerLinkedAnimInstance::Setup_CycleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	// 在循环状态下，设置自定义旋转模式为插值旋转
	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EInterpolation;
}

/*
void UCancerLinkedAnimInstance::Update_CycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;
	// 从选择器表中获取适合当前状态的动画序列
	CycleAnimData.AnimSequence = GetAnimSequence(this, CycleAnimChooserTable);
	// 将当前节点转换为序列播放器
	const FSequencePlayerReference& SequencePlayer = ConvertToSequencePlayer(Node);
	// 使用惯性混合设置动画序列，实现平滑的动画过渡
	// 这是Cycle状态内部动画切换时的惯性化混合，比如跑步循环动画，切换到了行走循环动画，不是状态间过渡时的惯性化设置
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, CycleAnimData.AnimSequence);
	// 虚幻引擎的距离匹配（Distance Matching），用于根据角色移动速度动态调整动画播放速率
	UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(SequencePlayer,
	                                                      GetParent()->LocomotionData.Movement.DisplacementSpeed);
}*/

void UCancerLinkedAnimInstance::UpdateMove_F(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UpdateCycleByDir(Context, Node, ECancerDirection_6::F);
}

void UCancerLinkedAnimInstance::UpdateMove_FL(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UpdateCycleByDir(Context, Node, ECancerDirection_6::FL);
}

void UCancerLinkedAnimInstance::UpdateMove_FR(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UpdateCycleByDir(Context, Node, ECancerDirection_6::FR);
}

void UCancerLinkedAnimInstance::UpdateMove_B(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UpdateCycleByDir(Context, Node, ECancerDirection_6::B);
}

void UCancerLinkedAnimInstance::UpdateMove_BL(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UpdateCycleByDir(Context, Node, ECancerDirection_6::BL);
}

void UCancerLinkedAnimInstance::UpdateMove_BR(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	UpdateCycleByDir(Context, Node, ECancerDirection_6::BR);
}

void UCancerLinkedAnimInstance::Setup_StopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	SetUpAnim(StopAnimData);

	StopAnimData.AnimSequence = GetAnimSequence(this, StopAnimChooserTable);

	if (!StopAnimData.AnimSequence)
		return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, StopAnimData.AnimSequence);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 1.55f);

	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EHold;
}

void UCancerLinkedAnimInstance::Update_StopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	// 检查角色是否没有了加速度
	if (!GetParent()->LocomotionData.States.bHasAcceleration)
	{
		// 检查角色是否仍在移动（没有加速度也是可以移动的，因为有惯性）
		if (GetParent()->LocomotionData.States.bHasVelocity)
		{
			// 预测角色在地面移动的停止距离
			const float StopDistance = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
				GetParentMovement()->Velocity,
				GetParentMovement()->bUseSeparateBrakingFriction,
				GetParentMovement()->BrakingFriction,
				GetParentMovement()->GroundFriction,
				GetParentMovement()->BrakingFrictionFactor,
				GetParentMovement()->BrakingDecelerationWalking
			).Length();

			// 如果停止距离大于阈值（5个单位），进行距离匹配
			if (StopDistance >= 5.f)
			{
				// 使用距离匹配技术，根据预测的停止距离调整动画播放位置
				UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, StopDistance,
				                                                    GetParent()->LocomotionConfig.Distance);
			}
		}
	}

	// 推进动画时间
	USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, 1.25f);
}

void UCancerLinkedAnimInstance::Setup_PivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	PivotAnimData.AnimSequence = GetAnimSequence(this, PivotAnimChooserTable);
	if (!PivotAnimData.AnimSequence) return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, PivotAnimData.AnimSequence);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.f);

	// 设置自定义旋转模式为动画驱动
	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EAnimRotation;

	// 准备自定义选择匹配数据
	PivotAnimData.CurrentAccelerationDirection = GetParent()->LocomotionData.Movement.Acceleration.GetSafeNormal2D().
	                                                          Rotation().Yaw;
	PivotAnimData.EntryAccelerationAngle = FRotator::NormalizeAxis(PivotAnimData.CurrentAccelerationDirection);
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("角度%f")
	                                                                     , PivotAnimData.CurrentAccelerationDirection));
	PivotAnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;
	PivotAnimData.TargetAngle = GetParent()->LocomotionData.Rotation.LocalAccelerationAngle;

	// 初始化Post状态的数据
	PostAnimData.AnimSequence = PivotAnimData.AnimSequence;
	PostAnimData.AnimStartPosition = 0.f;
	PostAnimData.TargetAngle = GetParent()->LocomotionData.Rotation.LocalAccelerationAngle;
}

void UCancerLinkedAnimInstance::Update_PivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	// 预测地面移动的回转距离（基于加速度、速度和摩擦计算）
	const float PivotDistance = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(
		GetParentMovement()->GetCurrentAcceleration(),
		GetParentMovement()->Velocity,
		GetParentMovement()->GroundFriction // 地面摩擦系数
	).Length();

	// 如果角色正在转身且有加速度，应用距离匹配
	if (GetParent()->LocomotionData.States.bIsPivoting && GetParent()->LocomotionData.States.bHasAcceleration)
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, PivotDistance,
		                                                    GetParent()->LocomotionConfig.Distance);
	}

	const float DeltaTime = Context.GetContext()->GetDeltaTime();
	FAnimExtractContext PostCtx(static_cast<double>(PivotAnimData.AnimStartPosition));
	const float RotationAlpha = PivotAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, PostCtx);

	CustomRotationMatching(RotationAlpha, DeltaTime, 10.f, GetParent()->LocomotionData,
	                       GetParent()->GetCancerCharacterMovementComponent()->CustomRotationData, PivotAnimData);

	// 指定Post状态的动画起始播放位置
	PostAnimData.AnimStartPosition = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);
}

void UCancerLinkedAnimInstance::Setup_PivotTestAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	PivotAnimData.AnimSequence = GetAnimSequence(this, PivotAnimChooserTable);
	if (!PivotAnimData.AnimSequence) return;

	const FSequencePlayerReference& SequencePlayer = ConvertToSequencePlayer(Node);

	USequencePlayerLibrary::SetSequence(SequencePlayer, PivotAnimData.AnimSequence);

	// 设置自定义旋转模式为动画驱动
	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EAnimRotation;

	// 准备自定义选择匹配数据
	PivotAnimData.CurrentAccelerationDirection = GetParent()->LocomotionData.Movement.Acceleration.GetSafeNormal2D().
	                                                          Rotation().Yaw;
	PivotAnimData.EntryAccelerationAngle = FRotator::NormalizeAxis(PivotAnimData.CurrentAccelerationDirection);
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("角度%f")
	                                                                     , PivotAnimData.CurrentAccelerationDirection));
	PivotAnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;
	PivotAnimData.TargetAngle = GetParent()->LocomotionData.Rotation.LocalAccelerationAngle;
}

void UCancerLinkedAnimInstance::Update_PivotTestAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;
	const FSequencePlayerReference& SequencePlayer = ConvertToSequencePlayer(Node);

	const float DeltaTime = Context.GetContext()->GetDeltaTime();
	const float CurrentTime = USequencePlayerLibrary::GetAccumulatedTime(SequencePlayer);
	FAnimExtractContext PivotCtx(static_cast<double>(CurrentTime));
	const float RotationAlpha = PivotAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, PivotCtx);

	CustomRotationMatching(RotationAlpha, DeltaTime, 10.f, GetParent()->LocomotionData,
	                       GetParent()->GetCancerCharacterMovementComponent()->CustomRotationData, PivotAnimData);
}

void UCancerLinkedAnimInstance::Setup_PostAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	if (!PostAnimData.AnimSequence) return;
	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, PostAnimData.AnimSequence);
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, PostAnimData.AnimStartPosition);

	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EAnimRotation;

	// 保存原始目标角度
	const float PostPivotDir = PostAnimData.TargetAngle;
	// 从动画曲线获取旋转混合权重值（在动画起始位置）
	FAnimExtractContext PostCtx(static_cast<double>(PostAnimData.AnimStartPosition));
	const float RotationAlpha = PostAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, PostCtx);
	// 根据旋转混合权重调整目标角度
	PostAnimData.TargetAngle = PostPivotDir * (1.f - RotationAlpha);
	// 计算当前加速度方向（转换为2D平面并获取偏航角）
	PostAnimData.CurrentAccelerationDirection = GetParent()->LocomotionData.Movement.Acceleration.GetSafeNormal2D().
	                                                         Rotation().Yaw;
	// 计算方向差异：本地加速度方向与目标角度之间的标准化差异
	const float DirectionDiff = FRotator::NormalizeAxis(
		GetParent()->LocomotionData.Rotation.LocalAccelerationAngle - PostAnimData.TargetAngle);
	// 计算进入时的加速度方向，考虑方向差异
	PostAnimData.EntryAccelerationAngle = FRotator::NormalizeAxis(
		PostAnimData.CurrentAccelerationDirection - DirectionDiff);
	// 保存进入时的角色旋转偏航角
	PostAnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;
}

void UCancerLinkedAnimInstance::Update_PostAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;
	if (!PostAnimData.AnimSequence) return;

	const FSequenceEvaluatorReference& SequenceEvaluator = ConvertToSequenceEvaluator(Node);

	const float DeltaTime = Context.GetContext()->GetDeltaTime();
	const float CurrentTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);
	// 获取起始位置的旋转混合权重值
	FAnimExtractContext StartCtx(static_cast<double>(PostAnimData.AnimStartPosition));
	const float StartRotationAlpha = PostAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, StartCtx);
	// 获取当前位置的旋转混合权重值
	FAnimExtractContext CurrCtx(static_cast<double>(CurrentTime));
	const float CurrentRotationAlpha = PostAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, CurrCtx);
	// 将旋转混合权重从[StartRotationAlpha, 1]范围映射到[0, 1]范围
	const float MappedRotationAlpha = FMath::GetMappedRangeValueClamped(FVector2f(StartRotationAlpha, 1.f),
	                                                                    FVector2f(0.f, 1.f), CurrentRotationAlpha);

	CustomRotationMatching(MappedRotationAlpha, DeltaTime, 10.f, GetParent()->LocomotionData,
	                       GetParentMovement()->CustomRotationData, PostAnimData);

	FVector2D PlayRateFinal = PlayRateDefault;
	// 如果当前动画位置的距离曲线值小于2，提高播放速率以实现快速过渡
	if (PostAnimData.AnimSequence->EvaluateCurveData(GetParent()->LocomotionConfig.Distance, CurrCtx) < 2.f)
	{
		PlayRateFinal.Y = 2.f;
	}

	const float Speed = GetParent()->LocomotionData.Movement.DisplacementSpeed;
	const float MaxSpeed = FMath::Max(GetParent()->LocomotionData.Movement.MaxSpeed, 1.f);
	const float SpeedScale = FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f);
	const float DisplacementScaled = GetParent()->LocomotionData.Movement.DisplacementSinceLastUpdate * SpeedScale * (
		1.f - MappedRotationAlpha);
	if (DisplacementScaled > KINDA_SMALL_NUMBER)
	{
		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(
			Context, SequenceEvaluator,
			DisplacementScaled,
			GetParent()->LocomotionConfig.Distance,
			PlayRateFinal
		);
	}
	else
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, PlayRateFinal.Y);
	}
}


void UCancerLinkedAnimInstance::Setup_CrossTurnAnim(const FAnimUpdateContext& Context,
                                                    const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	CrossTurnAnimData.AnimSequence = GetAnimSequence(this, CrossTurnAnimChooserTable);
	if (!CrossTurnAnimData.AnimSequence) return;

	const FSequencePlayerReference& SequencePlayer = ConvertToSequencePlayer(Node);

	USequencePlayerLibrary::SetSequence(SequencePlayer, CrossTurnAnimData.AnimSequence);

	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EAnimRotation;

	// 初始化Cross状态的数据
	CrossTurnAnimData.CurrentAccelerationDirection = GetParent()->
	                                                 LocomotionData.Movement.Acceleration.GetSafeNormal2D().
	                                                 Rotation().Yaw;
	CrossTurnAnimData.EntryAccelerationAngle = FRotator::NormalizeAxis(CrossTurnAnimData.CurrentAccelerationDirection);
	CrossTurnAnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;
	CrossTurnAnimData.TargetAngle = GetParent()->LocomotionData.Rotation.CrossTurnAngle;
}

void UCancerLinkedAnimInstance::Update_CrossTurnAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);

	const float DeltaTime = Context.GetContext()->GetDeltaTime();
	const float CurrentTime = USequencePlayerLibrary::GetAccumulatedTime(SequencePlayer);
	FAnimExtractContext CrossCtx(static_cast<double>(CurrentTime));
	const float RotationAlpha = CrossTurnAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, CrossCtx);

	// 当动画旋转结束(基本上为1)时，就可以认为转身结束，不需要等待后续恢复站立姿态的动画帧了
	if (RotationAlpha > 0.99f) GetParent()->LocomotionData.States.bCrossTurn = false;

	CustomRotationMatching(RotationAlpha, DeltaTime, 10.f, GetParent()->LocomotionData,
	                       GetParentMovement()->CustomRotationData, CrossTurnAnimData);
}

void UCancerLinkedAnimInstance::Setup_JumpStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);
	auto Animation = GetAnimSequence(this, JumpStartAnimChooserTable);
	USequencePlayerLibrary::SetSequence(SequencePlayer, Animation);
}

void UCancerLinkedAnimInstance::Setup_JumpStartLoopAnim(const FAnimUpdateContext& Context,
                                                        const FAnimNodeReference& Node)
{
	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);
	auto Animation = GetAnimSequence(this, JumpLoopAnimChooserTable);
	USequencePlayerLibrary::SetSequence(SequencePlayer, Animation);
}

void UCancerLinkedAnimInstance::Setup_FallLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);
	auto Animation = GetAnimSequence(this, FallLandAnimChooserTable);
	USequencePlayerLibrary::SetSequence(SequencePlayer, Animation);
}

void UCancerLinkedAnimInstance::Setup_Double(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);
	auto Animation = GetAnimSequence(this, DoubleJumpAnimChooserTable);
	USequencePlayerLibrary::SetSequence(SequencePlayer, Animation);
}

void UCancerLinkedAnimInstance::Setup_TurnInPlace(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	TurnInPlaceAnimData.CurrentAccelerationDirection = GetParent()->
	                                                   LocomotionData.Movement.Acceleration.GetSafeNormal2D().
	                                                   Rotation().Yaw;
	TurnInPlaceAnimData.EntryAccelerationAngle =
		FRotator::NormalizeAxis(CrossTurnAnimData.CurrentAccelerationDirection);
	TurnInPlaceAnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;
	TurnInPlaceAnimData.TargetAngle = -GetParent()->LocomotionData.Rotation.RootYawOffset;

	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);
	TurnInPlaceAnimData.AnimSequence = GetAnimSequence(this, TurnPlaceAnimChooserTable);
	if (!TurnInPlaceAnimData.AnimSequence)
		return;
	TurnInPlaceRecoveryAnimData.AnimSequence = TurnInPlaceAnimData.AnimSequence;
	USequencePlayerLibrary::SetSequence(SequencePlayer, TurnInPlaceAnimData.AnimSequence);
}

void UCancerLinkedAnimInstance::Update_TurnInPlace(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;

	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);

	const float DeltaTime = Context.GetContext()->GetDeltaTime();
	const float CurrentTime = USequencePlayerLibrary::GetAccumulatedTime(SequencePlayer);
	FAnimExtractContext TurnInPlaceCtx(static_cast<double>(CurrentTime));
	const float RotationAlpha = TurnInPlaceAnimData.AnimSequence->EvaluateCurveData(
		GetParent()->LocomotionConfig.RotationAlpha, TurnInPlaceCtx);

	CustomRotationMatching(RotationAlpha, DeltaTime, 10.f, GetParent()->LocomotionData,
	                       GetParentMovement()->CustomRotationData, TurnInPlaceAnimData);

	//指定TurnInPlaceRecovery的开始位置
	TurnInPlaceRecoveryAnimData.AnimStartPosition = USequencePlayerLibrary::GetAccumulatedTime(SequencePlayer);
}

void UCancerLinkedAnimInstance::Setup_TurnInPlaceRecovery(const FAnimUpdateContext& Context,
                                                          const FAnimNodeReference& Node)
{
	if (!GetParentMovement() || !GetParent()) return;
	const FSequencePlayerReference& SequencePlayer = ConvertToSequencePlayer(Node);
	if (!TurnInPlaceRecoveryAnimData.AnimSequence) return;
	USequencePlayerLibrary::SetSequence(SequencePlayer, TurnInPlaceRecoveryAnimData.AnimSequence);
	USequencePlayerLibrary::SetStartPosition(SequencePlayer, TurnInPlaceRecoveryAnimData.AnimStartPosition);
}

void UCancerLinkedAnimInstance::Update_TurnInPlaceRecovery(const FAnimUpdateContext& Context,
                                                           const FAnimNodeReference& Node)
{
}


void UCancerLinkedAnimInstance::SetUpAnim(FCurrentAnimData& AnimData)
{
	AnimData.EntryVelocityAngle = GetParent()->LocomotionData.Rotation.LocalVelocityDirectionAngle;
	AnimData.EntryAccelerationAngle = GetParent()->LocomotionData.Rotation.LocalAccelerationAngle;
	AnimData.EntryVelocityDirection_8 = CalculateVelocityDirection(AnimData.EntryVelocityAngle);
	AnimData.EntryVelocityDirection = GetParent()->LocomotionData.Rotation.VelocityDirection;
	AnimData.EntryGait = GetParent()->LocomotionData.States.Gait;
	AnimData.EntryRotationYaw = GetParent()->LocomotionData.Rotation.ActorRotation.Yaw;
}

void UCancerLinkedAnimInstance::UpdateCycleByDir(const FAnimUpdateContext& Context,
                                                 const FAnimNodeReference& Node, ECancerDirection_6 Direction)
{
	GetParentMovement()->CustomRotationData.CustomRotationMode = ECustomRotationMode::EInterpolation;
	const FSequencePlayerReference SequencePlayer = ConvertToSequencePlayer(Node);
	if (!CycleAnimChooserTable)
	{
		return;
	}

	FCancerDirection_6 Direction_6(GetParent()->LocomotionData.States.Gait, Direction);

	// 1) 构建评估上下文（C++ 版本）
	FChooserEvaluationContext EvalCtx = UChooserFunctionLibrary::MakeChooserEvaluationContext();

	// 2) 用 InstancedStruct 注入你的结构体（替代 Blueprint 的 AddChooserStructInput）
	FInstancedStruct DirInput;
	DirInput.InitializeAs<FCancerDirection_6>();
	DirInput.GetMutable<FCancerDirection_6>() = Direction_6;
	EvalCtx.Params.Add(DirInput);

	EvalCtx.AddObjectParam(this);

	// 3) 将 ChooserTable 包装为可评估的 FInstancedStruct
	FInstancedStruct EvaluateStruct = UChooserFunctionLibrary::MakeEvaluateChooser(CycleAnimChooserTable);

	// 4) 评估并返回动画（UAnimSequenceBase 以兼容 Montage/Sequence）
	UObject* ResultObj = UChooserFunctionLibrary::EvaluateObjectChooserBase(
		EvalCtx,
		EvaluateStruct,
		UAnimSequenceBase::StaticClass(),
		/*bResultIsClass*/ false
	);

	UAnimSequenceBase* Anim = Cast<UAnimSequenceBase>(ResultObj);
	if (!Anim)
	{
		return;
	}

	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, Anim);

	UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(
		SequencePlayer,
		GetParent()->LocomotionData.Movement.DisplacementSpeed
	);
}

ECancerDirection_8 UCancerLinkedAnimInstance::CalculateVelocityDirection(float Angle) const
{
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, -22.5f, 22.5f, true, true))
	{
		return ECancerDirection_8::F;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, -67.5f, -22.5f, true, false))
	{
		return ECancerDirection_8::FL;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, -112.5f, -67.5f, true, false))
	{
		return ECancerDirection_8::L;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, -157.5f, -112.5f, true, false))
	{
		return ECancerDirection_8::BL;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, 112.5f, 157.5f, true, false))
	{
		return ECancerDirection_8::BR;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, 67.5f, 112.5f, true, false))
	{
		return ECancerDirection_8::R;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(
		Angle, 22.5f, 67.5f, true, false))
	{
		return ECancerDirection_8::FR;
	}
	return ECancerDirection_8::B;
}


#pragma region 辅助函数

UAnimSequenceBase* UCancerLinkedAnimInstance::GetAnimSequence(const UObject* ContextObject,
                                                              UChooserTable* AnimChooserTable)
{
	check(AnimChooserTable);

	if (!ContextObject) return nullptr;

	FChooserEvaluationContext EvalCtx = UChooserFunctionLibrary::MakeChooserEvaluationContext();
	EvalCtx.AddObjectParam(const_cast<UObject*>(ContextObject));

	FInstancedStruct ChooserInst = UChooserFunctionLibrary::MakeEvaluateChooser(AnimChooserTable);
	UObject* ResultObj = UChooserFunctionLibrary::EvaluateObjectChooserBase(
		EvalCtx,
		ChooserInst,
		UAnimSequenceBase::StaticClass(),
		/*bResultIsClass*/ false
	);
	UAnimSequenceBase* Anim = Cast<UAnimSequenceBase>(ResultObj);

	return Anim;
}

UCancerAnimInstance* UCancerLinkedAnimInstance::GetParent() const
{
	return Parent.Get();
}

UCancerAnimInstance* UCancerLinkedAnimInstance::GetParentUnsafe() const
{
	return Parent.Get();
}

UCancerMovementComponent* UCancerLinkedAnimInstance::GetParentMovement() const
{
	if (Character)
	{
		return Cast<UCancerMovementComponent>(Character->GetCharacterMovement());
	}
	return nullptr;
}
#pragma endregion
UE_ENABLE_OPTIMIZATION
