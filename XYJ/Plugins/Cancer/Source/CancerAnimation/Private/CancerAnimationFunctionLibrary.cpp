#include "CancerAnimationFunctionLibrary.h"

#include "CancerAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "Animation/AnimCurveCompressionCodec_UniformIndexable.h"
#include "AnimNodes/AnimNode_SequenceEvaluator.h"
#include "Components/CancerLocomotionComponent.h"
#include "Components/CancerMovementComponent.h"

DECLARE_LOG_CATEGORY_CLASS(CancerAnimationFunctionLog, Log, All);


void UCancerAnimationFunctionLibrary::PrintStrOnThreadSafe(const FString& Str, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, -1, Color, Str);
		UE_LOG(CancerAnimationFunctionLog, Warning, TEXT("%s"), *Str);
	}
}

FCancerVelocityBlend UCancerAnimationFunctionLibrary::CalculateVelocityBlend(const AActor* Actor)
{
	// 将世界空间速度转换为局部空间速度
	FVector LocRelativeVelocityDir = UKismetMathLibrary::LessLess_VectorRotator(
		UKismetMathLibrary::Normal(Actor->GetVelocity()),
		Actor->GetActorRotation());

	// 计算速度分量的绝对值之和
	float Sum =
		UKismetMathLibrary::Abs(LocRelativeVelocityDir.X) +
		UKismetMathLibrary::Abs(LocRelativeVelocityDir.Y) +
		UKismetMathLibrary::Abs(LocRelativeVelocityDir.Z);

	// 计算归一化后的相对速度方向
	FVector RelativeCancerDirection = LocRelativeVelocityDir / Sum;

	// 创建并初始化速度混合值
	FCancerVelocityBlend CurVelocityBlend = FCancerVelocityBlend();

	// 计算各方向的混合权重，范围限制在0到1之间
	CurVelocityBlend.VelocityF = UKismetMathLibrary::FClamp(RelativeCancerDirection.X, 0, 1);
	CurVelocityBlend.VelocityB = UKismetMathLibrary::Abs(UKismetMathLibrary::FClamp(RelativeCancerDirection.X, -1, 0));
	CurVelocityBlend.VelocityR = UKismetMathLibrary::FClamp(RelativeCancerDirection.Y, 0, 1);
	CurVelocityBlend.VelocityL = UKismetMathLibrary::Abs(UKismetMathLibrary::FClamp(RelativeCancerDirection.Y, -1, 0));

	return CurVelocityBlend;
}


FCancerVelocityBlend UCancerAnimationFunctionLibrary::InterpVelocityBlend(FCancerVelocityBlend Current,
                                                                          FCancerVelocityBlend Target,
                                                                          float InterpSpeed, float DeltaTime)
{
	FCancerVelocityBlend CurVelocityBlend = FCancerVelocityBlend();

	// 对每个方向的混合权重进行插值
	CurVelocityBlend.VelocityF =
		UKismetMathLibrary::FInterpTo(Current.VelocityF, Target.VelocityF, InterpSpeed, DeltaTime);
	CurVelocityBlend.VelocityB =
		UKismetMathLibrary::FInterpTo(Current.VelocityB, Target.VelocityB, InterpSpeed, DeltaTime);
	CurVelocityBlend.VelocityR =
		UKismetMathLibrary::FInterpTo(Current.VelocityR, Target.VelocityR, InterpSpeed, DeltaTime);
	CurVelocityBlend.VelocityL =
		UKismetMathLibrary::FInterpTo(Current.VelocityL, Target.VelocityL, InterpSpeed, DeltaTime);

	return CurVelocityBlend;
}

float UCancerAnimationFunctionLibrary::CalculateDirectionVelocityAngle(const FVector& Dir, const AActor* Actor)
{
	/*return UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::Conv_VectorToRotator(CharacterPtr->GetVelocity())
				,Actor->GetActorRotation()).Yaw;*/
	return UKismetAnimationLibrary::CalculateDirection(Dir, Actor->GetActorRotation());;
}

float UCancerAnimationFunctionLibrary::CalculateDirectionAccelerationAngle(const FVector& Dir, const AActor* Actor)
{
	return UKismetAnimationLibrary::CalculateDirection(Dir, Actor->GetActorRotation());
}

ECancerDirection_4 UCancerAnimationFunctionLibrary::CalculateQuadrant(ECancerDirection_4 Current, float FR, float FL,
                                                                      float BR, float BL, float Buffer, float Angle)
{
	bool IncreaseBuffer = Current != ECancerDirection_4::Forward || Current != ECancerDirection_4::Backward;
	if (AngleInRange(Angle, FL, FR, Buffer, IncreaseBuffer))
	{
		return ECancerDirection_4::Forward;
	}
	IncreaseBuffer = Current != ECancerDirection_4::Left || Current != ECancerDirection_4::Right;
	if (AngleInRange(Angle, FR, BR, Buffer, IncreaseBuffer))
	{
		return ECancerDirection_4::Right;
	}

	if (AngleInRange(Angle, BL, FL, Buffer, IncreaseBuffer))
	{
		return ECancerDirection_4::Left;
	}
	return ECancerDirection_4::Backward;
}

ECancerDirection_8 UCancerAnimationFunctionLibrary::CalculateDirByAngle(float Angle)
{

	if (UKismetMathLibrary::InRange_FloatFloat(Angle,-22.5,22.5,true,true))
	{
		return ECancerDirection_8::F;
	}

	if (UKismetMathLibrary::InRange_FloatFloat(Angle,-67.5,-22.5,true,false))
	{
		return ECancerDirection_8::FL;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(Angle,-112.5,-67.5,true,false))
	{
		return ECancerDirection_8::L;
	}

	if (UKismetMathLibrary::InRange_FloatFloat(Angle,-157.5,-112.5,true,false))
	{
		return ECancerDirection_8::BL;
	}

	if (UKismetMathLibrary::InRange_FloatFloat(Angle,67.5,112.5,true,false))
	{
		return ECancerDirection_8::R;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(Angle,22.5,67.5,false,false))
	{
		return ECancerDirection_8::FR;
	}
	return ECancerDirection_8::B;
}

bool UCancerAnimationFunctionLibrary::AngleInRange(float Angle, float Min, float Max, float Buffer, bool IncreaseBuffer)
{
	if (IncreaseBuffer)
	{
		// 增加角度范围
		return UKismetMathLibrary::InRange_FloatFloat(Angle, Min - Buffer, Max + Buffer, true, true);
	}
	// 缩小角度范围
	return UKismetMathLibrary::InRange_FloatFloat(Angle, Min + Buffer, Max - Buffer, true, true);
}

float UCancerAnimationFunctionLibrary::GetAnimationCurveValueFromCharacter(const ACharacter* Character,
                                                                           const FName& CurveName)
{
	const auto* Mesh{IsValid(Character) ? Character->GetMesh() : nullptr};
	const auto* AnimationInstance{IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr};

	return IsValid(AnimationInstance) ? AnimationInstance->GetCurveValue(CurveName) : 0.0f;
}

FVector UCancerAnimationFunctionLibrary::PredictGroundStopLocation(const UCharacterMovementComponent* MovementComponent)
{
	FVector PredictedStopLocation = FVector::ZeroVector;

	// 计算实际使用的刹车摩擦力
	// 如果启用了独立刹车摩擦力，则使用BrakingFriction，否则使用GroundFriction
	float ActualBrakingFriction = (MovementComponent->bUseSeparateBrakingFriction
		                               ? MovementComponent->BrakingFriction
		                               : MovementComponent->GroundFriction);

	// 确保摩擦力系数和摩擦力值都是非负数
	const float FrictionFactor = FMath::Max(0.f, MovementComponent->BrakingFrictionFactor);
	ActualBrakingFriction = FMath
		::Max(
			0.f
			,
			ActualBrakingFriction * FrictionFactor
		);

	// 确保刹车减速值是非负数
	float BrakingDeceleration = FMath::Max(0.f, MovementComponent->BrakingDecelerationWalking);

	//忽略Z轴
	const FVector Velocity2D = MovementComponent->Velocity * FVector(1.f, 1.f, 0.f);

	FVector VelocityDir2D;
	float Speed2D;
	//ToDirectionAndLength  用于将此向量转换为单位方向向量及其原始长度。形参:OutDir — 传入以存储单位方向向量的引用。OutLength — 传入以存储向量长度的引用。
	Velocity2D.ToDirectionAndLength(VelocityDir2D, Speed2D);

	// 计算停止所需的总阻力
	const float Divisor = ActualBrakingFriction * Speed2D + BrakingDeceleration;

	// 如果总阻力大于0，计算停止时间和最终位置
	if (Divisor > 0.f)
	{
		// 计算停止所需时间
		const float TimeToStop = Speed2D / Divisor;

		// 计算最终停止位置
		// 公式：s = vt + 0.5at²
		// 其中：v是初始速度，a是加速度（这里是负的阻力），t是时间

		PredictedStopLocation = Velocity2D * TimeToStop + 0.5f *
			((-ActualBrakingFriction) * Velocity2D - BrakingDeceleration * VelocityDir2D) * TimeToStop * TimeToStop;
	}
	//UE_LOG(LogTemp, Display, TEXT("预测停止距离: %f"), UKismetMathLibrary::VSizeXY(PredictedStopLocation));
	return PredictedStopLocation;
}

FVector UCancerAnimationFunctionLibrary::PredictGroundPivotLocation(
	const UCharacterMovementComponent* MovementComponent)
{
	// 初始化预测转向位置为零向量
	FVector PredictedPivotLocation = FVector::ZeroVector;

	// 忽略Z轴
	const FVector Acceleration2D = MovementComponent->GetCurrentAcceleration() * FVector(1.f, 1.f, 0.f);

	// 获取加速度方向和大小
	FVector AccelerationDir2D;
	float AccelerationSize2D;
	Acceleration2D.ToDirectionAndLength(AccelerationDir2D, AccelerationSize2D);

	// 计算速度在加速度方向上的分量
	const float VelocityAlongAcceleration = (MovementComponent->Velocity | AccelerationDir2D);

	// 如果速度分量小于0（即速度方向与加速度方向相反），进行转向预测
	if (VelocityAlongAcceleration < 0.0f)
	{
		// 获取速度分量的绝对值
		const float SpeedAlongAcceleration = -VelocityAlongAcceleration;

		// 计算转向所需的总阻力
		const float Divisor = AccelerationSize2D + 2.f * SpeedAlongAcceleration * MovementComponent->GroundFriction;

		// 计算转向所需时间
		const float TimeToDirectionChange = SpeedAlongAcceleration / Divisor;

		// 计算转向力
		const FVector AccelerationForce = MovementComponent->GetCurrentAcceleration() -
			(MovementComponent->Velocity - AccelerationDir2D * MovementComponent->Velocity.Size2D()) * MovementComponent
			->GroundFriction;

		// 计算预测转向位置
		// 公式：s = vt + 0.5at²
		// 其中：v是初始速度，a是加速度（这里是转向力），t是时间
		PredictedPivotLocation = MovementComponent->Velocity * TimeToDirectionChange + 0.5f * AccelerationForce *
			TimeToDirectionChange *
			TimeToDirectionChange;
	}

	return PredictedPivotLocation;
}

FSequenceEvaluatorReference UCancerAnimationFunctionLibrary::AdvanceTimeByRemainingTurnYaw(
	const FAnimUpdateContext& UpdateContext, const FSequenceEvaluatorReference& SequenceEvaluator, float InYaw,
	FName RemainingTurnYawCurveName, FVector2D PlayRateClamp)
{
	SequenceEvaluator.CallAnimNodeFunction<FAnimNode_SequenceEvaluator>(
		TEXT("AdvanceTimeByRemainingTurnYaw"),
		[&UpdateContext, InYaw, RemainingTurnYawCurveName, PlayRateClamp](
		FAnimNode_SequenceEvaluator& InSequenceEvaluator)
		{
			if (const FAnimationUpdateContext* AnimationUpdateContext = UpdateContext.GetContext())
			{
				if (const UAnimSequenceBase* AnimSequence = Cast<UAnimSequence>(InSequenceEvaluator.GetSequence()))
				{
					const float NewTime = GetAnimPositionFromYaw(AnimSequence, InYaw, RemainingTurnYawCurveName);
					if (!InSequenceEvaluator.SetExplicitTime(NewTime))
					{
						UE_LOG(CancerAnimationFunctionLog, Warning,
						       TEXT(
							       "Could not set explicit time on sequence evaluator, value is not dynamic. Set it as Always Dynamic."
						       ));
					}
					UE_LOG(CancerAnimationFunctionLog, Display, TEXT("RemainingTurnYaw 新的时间： %f"), NewTime);
				}
			}
		});
	return SequenceEvaluator;
}

float UCancerAnimationFunctionLibrary::GetAnimPositionFromYaw(const UAnimSequenceBase* InAnimSequence,
                                                              const float& InYaw, FName InCurveName)
{
	// 检查动画序列指针是否为空，防止空指针异常
	if (InAnimSequence == nullptr)
	{
		return 0.f;
	}
	// 创建 FAnimCurveBufferAccess 实例，用于访问指定名称的动画曲线
	FAnimCurveBufferAccess BufferCurveAccess(InAnimSequence, InCurveName);
	// 检查动画曲线访问器是否有效
	if (BufferCurveAccess.IsValid())
	{
		// 获取动画曲线采样点的数量
		const int32 NumKeys = BufferCurveAccess.GetNumSamples();
		// 如果采样点数量少于 2 个，无法进行插值计算，直接返回 0
		if (NumKeys < 2)
		{
			return 0.f;
		}

		if (FMath::Abs(InYaw) < 5.f)
		{
			return 0.f;
		}

		// 二分查找的假设条件：
		// 1. 每个采样点的值唯一，即给定一个值，在动画时间轴上对应唯一位置
		// 2. 采样点的值按升序排列

		int32 First = 0; // 二分查找的起始索引，从第一个采样点开始
		int32 Last = NumKeys - 1; // 二分查找的结束索引，为最后一个采样点
		int32 Count = Last - First; // 待查找区间的长度

		// 二分查找循环，直到查找区间长度为 0
		while (Count > 0)
		{
			int32 Step = Count / 2; // 计算中间步长
			int32 Middle = First + Step; // 计算中间索引

			// 如果目标距离大于中间采样点的值，调整查找区间为后半部分
			float Value = BufferCurveAccess.GetValue(Middle);
			if (InYaw > Value)
			{
				First = Middle + 1; // 更新起始索引
				Count -= Step + 1; // 更新待查找区间长度
			}
			else
			{
				Count = Step; // 否则，调整查找区间为前半部分
			}
		}
		if (First < 1)
		{
			return 0.f;
		}

		// 获取找到的区间的前一个和当前采样点的值
		const float KeyAValue = BufferCurveAccess.GetValue(First - 1);
		const float KeyBValue = BufferCurveAccess.GetValue(First);
		// 计算两个采样点值的差值
		const float Diff = KeyBValue - KeyAValue;
		// 计算插值因子 Alpha，避免除以零的情况
		const float Alpha = !FMath::IsNearlyZero(Diff) ? ((InYaw - KeyAValue) / Diff) : 0.f;

		// 获取两个采样点对应的动画时间
		const float KeyATime = BufferCurveAccess.GetTime(First - 1);
		const float KeyBTime = BufferCurveAccess.GetTime(First);
		// 使用线性插值计算最终的动画时间位置
		return FMath::Lerp(KeyATime, KeyBTime, Alpha);
	}
	// 如果动画曲线访问器无效，返回 0
	return 0.f;
}

UCancerAnimInstance* UCancerAnimationFunctionLibrary::K2_GetCancerAnimInstance(AActor* Actor, bool& bIsValid,
                                                                               TSubclassOf<UCancerAnimInstance>
                                                                               OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerAnimInstance(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerAnimInstance::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

UCancerMovementComponent* UCancerAnimationFunctionLibrary::K2_GetCancerMovementComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerMovementComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerMovementComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerMovementComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

UCancerLocomotionComponent* UCancerAnimationFunctionLibrary::K2_CancerLocomotionComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerLocomotionComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerLocomotionComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerLocomotionComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}
