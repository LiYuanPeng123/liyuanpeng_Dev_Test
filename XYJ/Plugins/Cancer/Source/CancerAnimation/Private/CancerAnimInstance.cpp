#include "CancerAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "CancerAnimationFunctionLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Components/CancerMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/DataValidation.h"

void UCancerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (AActor* OwningActor = GetOwningActor())
	{
		CharacterPtr = Cast<ACharacter>(OwningActor);

		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}

#if WITH_EDITOR
	const auto* World{GetWorld()};

	if (IsValid(World) && !World->IsGameWorld())
	{
		// Use default objects for editor preview.
		if (!CharacterPtr.IsValid())
		{
			CharacterPtr = GetMutableDefault<ACharacter>();
		}
	}
#endif
}

void UCancerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (!CharacterPtr.Get())return;
	if (!GetCancerCharacterMovementComponent()) return;

#pragma region 同步角色状态
	FGameplayTag NewGaitTag = GetCancerCharacterMovementComponent()->GetCurrentGaitTag();

	LocomotionData.States.Gait = NewGaitTag == GaitTag::Walk
		                             ? ECancerGait::Walk
		                             : (NewGaitTag == GaitTag::Run ? ECancerGait::Run : ECancerGait::Sprint);

	LocomotionData.States.bOrientToMovement = GetCancerCharacterMovementComponent()->GetCurrentRotationMode();
	LocomotionData.States.bStand = GetCancerCharacterMovementComponent()->GetCurrentStand();
#pragma endregion

#pragma region 更新旋转
	LocomotionData.Rotation.YawDeltaSinceLastUpdate =
		FRotator::NormalizeAxis(GetCharacterMovementComponent()->UpdatedComponent->
		                                                         GetComponentRotation().Yaw - LocomotionData.Rotation.
			ActorRotation.Yaw);
	LocomotionData.Rotation.YawDeltaSpeed = UKismetMathLibrary::SafeDivide
		(LocomotionData.Rotation.YawDeltaSinceLastUpdate, DeltaSeconds);

	LocomotionData.Rotation.ActorRotation = CharacterPtr->GetActorRotation();

	if (LocomotionData.bFirstUpdate)
	{
		LocomotionData.Rotation.YawDeltaSinceLastUpdate = 0.0f;
	}
	LocomotionData.Rotation.AimPitch = UKismetMathLibrary::NormalizeAxis(
		TryGetPawnOwner()->GetBaseAimRotation().Pitch);


#pragma endregion

#pragma region 更新位置
	LocomotionData.Movement.DisplacementSinceLastUpdate =
		UKismetMathLibrary::VSizeXY(CharacterPtr->GetActorLocation() - LocomotionData.Movement.WorldLocation);
	LocomotionData.Movement.DisplacementSpeed =
		UKismetMathLibrary::SafeDivide(LocomotionData.Movement.DisplacementSinceLastUpdate, DeltaSeconds);

	LocomotionData.Movement.WorldLocation = CharacterPtr->GetActorLocation();

	if (LocomotionData.bFirstUpdate)
	{
		LocomotionData.Movement.DisplacementSinceLastUpdate = 0.0f;
		LocomotionData.Movement.DisplacementSpeed = 0.0f;
	}
#pragma endregion

#pragma region 更新速度
	LocomotionData.Movement.Velocity = CharacterPtr->GetVelocity();
	LocomotionData.Movement.Velocity2D = LocomotionData.Movement.Velocity * FVector(1, 1, 0);
	LocomotionData.Movement.LocalVelocity2D = UKismetMathLibrary::LessLess_VectorRotator(
		LocomotionData.Movement.Velocity2D, LocomotionData.Rotation.ActorRotation);
	LocomotionData.Movement.Speed = LocomotionData.Movement.Velocity2D.Length();
	LocomotionData.Movement.MaxSpeed = GetCharacterMovementComponent()->GetMaxSpeed();

	LocomotionData.States.bHasVelocity = !
		UKismetMathLibrary::NearlyEqual_FloatFloat(UKismetMathLibrary::VSizeXYSquared(
			                                           LocomotionData.Movement.Velocity2D), 0.0f);


	LocomotionData.Rotation.LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(
		LocomotionData.Movement.Velocity2D, LocomotionData.Rotation.ActorRotation);

	LocomotionData.Rotation.LocalVelocityDirectionAngleWithOffset =
		LocomotionData.Rotation.LocalVelocityDirectionAngle - LocomotionData.Rotation.RootYawOffset;

	LocomotionData.Rotation.VelocityDirection = UCancerAnimationFunctionLibrary::CalculateQuadrant(
		LocomotionData.Rotation.VelocityDirection, 70, -70, 110, -110, 5,
		LocomotionData.Rotation.LocalVelocityDirectionAngle);

	LocomotionData.Rotation.VelocityDirectionWithOffset = UCancerAnimationFunctionLibrary::CalculateQuadrant(
		LocomotionData.Rotation.VelocityDirectionWithOffset, 70, -70, 110, -110, 5,
		LocomotionData.Rotation.LocalVelocityDirectionAngleWithOffset);

	//速度
	LocomotionData.Movement.VelocityBlend = UCancerAnimationFunctionLibrary::InterpVelocityBlend(
		LocomotionData.Movement.VelocityBlend,
		UCancerAnimationFunctionLibrary::CalculateVelocityBlend(
			CharacterPtr.Get()),
		LocomotionConfig.VelocityBlendInterpSpeed, DeltaSeconds);

	// 记录最后非零加速度（用于十字交叉转身）
	if (!LocomotionData.Movement.Acceleration.IsNearlyZero())
	{
		LocomotionData.Movement.LastNonZeroAcceleration = LocomotionData.Movement.Acceleration;
	}
#pragma endregion

#pragma region 更新加速度
	LocomotionData.Movement.Acceleration = GetCharacterMovementComponent()->GetCurrentAcceleration();
	LocomotionData.Movement.Acceleration2D = LocomotionData.Movement.Acceleration * FVector(1, 1, 0);

	LocomotionData.Movement.LocalAcceleration2D = UKismetMathLibrary::LessLess_VectorRotator(
		LocomotionData.Movement.Acceleration2D, LocomotionData.Rotation.ActorRotation);

	LocomotionData.States.bHasAcceleration = !UKismetMathLibrary::NearlyEqual_FloatFloat(
		UKismetMathLibrary::VSizeXYSquared(LocomotionData.Movement.LocalAcceleration2D), 0.0f);

	LocomotionData.Rotation.LocalAccelerationAngle = UKismetAnimationLibrary::CalculateDirection(
		LocomotionData.Movement.Acceleration2D, LocomotionData.Rotation.ActorRotation);

	LocomotionData.Rotation.AccelerationDirection = UCancerAnimationFunctionLibrary::CalculateQuadrant(
		LocomotionData.Rotation.AccelerationDirection, 45, -45, 135, -135, 5,
		LocomotionData.Rotation.LocalAccelerationAngle);
#pragma endregion

#pragma region 更新YawOffset
	if (GetCancerCharacterMovementComponent()->CustomRotationData.CustomRotationMode
		== ECustomRotationMode::EHold)
	{
		SetRootYawOffset(LocomotionData.Rotation.RootYawOffset -
			LocomotionData.Rotation.YawDeltaSinceLastUpdate);
	}
	else /*if (GetCancerCharacterMovementComponent()->CustomRotationData.CustomRotationMode
		== ECustomRotationMode::EInterpolation)*/
	{
		float TempYawOffset = UKismetMathLibrary::FloatSpringInterp(
			LocomotionData.Rotation.RootYawOffset, 0.f, LocomotionConfig.SpringState, 80.f, 1.f,
			DeltaSeconds, 1.f, 0.5f);
		SetRootYawOffset(TempYawOffset);
	}
	GetCancerCharacterMovementComponent()->CustomRotationData.CustomRotationMode
		= ECustomRotationMode::EInterpolation;
#pragma endregion

#pragma region 更新折返转身

	/* 回转开始条件检测：运动中，加速度方向突然与速度方向相反了(夹角 > |±120|，点积 < -0.5) */
	if (LocomotionData.Movement.Acceleration.GetSafeNormal2D().Dot(
			LocomotionData.Movement.Velocity.GetSafeNormal2D()) <
		-0.5f)
	{
		// 当前速度足够快（大于最大速度的50%）
		if (LocomotionData.Movement.Speed > LocomotionData.Movement.MaxSpeed * 0.5f)
		{
			// 若当前不是回转状态且有加速度 
			if (!LocomotionData.States.bIsPivoting && LocomotionData.States.bHasAcceleration
				&& LocomotionData.States.bOrientToMovement)
			{
				// 则可以进行回转运动
				LocomotionData.States.bIsPivoting = true;
			}
		}
	}
	/* 回转结束条件检测：加速度方向与速度方向夹角 ≤ |±90°|，即Dot ≥ 0时，表示输入与运动方向一致 */
	if (LocomotionData.Movement.Acceleration.GetSafeNormal2D().Dot(
			LocomotionData.Movement.Velocity.GetSafeNormal2D())
		>= 0.f)
	{
		// 若当前处于回转状态，但没有了加速度
		if (LocomotionData.States.bIsPivoting && !LocomotionData.States.bHasAcceleration)
		{
			// 结束回转运动
			LocomotionData.States.bIsPivoting = false;
		}
		// 补充条件：若惯性导致的移动，也结束回转
		if (LocomotionData.States.bHasVelocity)
		{
			LocomotionData.States.bIsPivoting = false;
		}
	}
	//强制结束
	if (!LocomotionData.States.bOrientToMovement)
	{
		LocomotionData.States.bIsPivoting = false;
	}


#pragma endregion

#pragma region 十字交叉转身


	/*// 没有加速度 并且 处于自由移动
	if (!LocomotionData.States.bHasAcceleration && LocomotionData.States.bOrientToMovement)
	{
		// 不管前面是否进行十字交叉转身，先停止
		LocomotionData.States.bCrossTurn = false;

		// 帧位移速度小于最大速度的1/3时
		if (LocomotionData.Movement.DisplacementSpeed < LocomotionData.Movement.MaxSpeed * 0.3f)
		{
			// 计算最后非零加速度方向与当前旋转的偏角
			const float YawOffset = FRotator::NormalizeAxis(
				LocomotionData.Movement.LastNonZeroAcceleration.Rotation().Yaw - LocomotionData.Rotation.ActorRotation.
				Yaw);
			// 若偏角超过45度时触发交叉转向
			if (FMath::Abs(YawOffset) >= 30.f)
			{
				LocomotionData.States.bCrossTurn = true;
				LocomotionData.Rotation.CrossTurnAngle = YawOffset;
			}
		}
	}*/

	// 1.有加速  2.移动时取消交叉转向 3.取消处于自由移动
	if (LocomotionData.States.bHasAcceleration || LocomotionData.States.bHasVelocity || !LocomotionData.States.
		bOrientToMovement)
	{
		LocomotionData.States.bCrossTurn = false;
		LocomotionData.Rotation.CrossTurnAngle = 0.f;
	}


#pragma endregion

#pragma region 更新 空中 状态
	LocomotionData.States.IsOnGround = GetCharacterMovementComponent()->IsMovingOnGround();
	LocomotionData.States.IsJumping = false;
	LocomotionData.States.IsFalling = false;
	if (GetCharacterMovementComponent()->MovementMode == MOVE_Falling)
	{
		if (CharacterPtr->GetVelocity().Z > 0.0f)
		{
			LocomotionData.States.IsJumping = true;
		}
		else
		{
			LocomotionData.States.IsFalling = true;
		}
	}
	if (!LocomotionData.States.IsOnGround)
	{
		LocomotionData.Movement.GroundDistance = GetCancerCharacterMovementComponent()->GetGroundInfo().GroundDistance;
	}
	else
	{
		LocomotionData.Movement.GroundDistance = 0.0f;
	}
	if (LocomotionData.States.IsJumping)
	{
		LocomotionData.Movement.TimeToJumpApex = UKismetMathLibrary::SafeDivide((0 - CharacterPtr->GetVelocity().Z),
			GetCharacterMovementComponent()->GetGravityZ());
	}
	else
	{
		LocomotionData.Movement.TimeToJumpApex = 0.0f;
	}
#pragma endregion

#pragma region 判断角色是否撞到了墙壁

	//This logic guesses if the character is running into a wall by
	//checking if there's a large angle between velocity and acceleration
	//(i.e. the character is pushing towards the wall but actually sliding to the side)
	//and if the character is trying to accelerate but speed is relatively low.
	//这种逻辑是通过检查速度与加速度之间的夹角是否较大
	//（即角色正朝着墙壁移动但实际上是向侧面滑动）以及角色是否试图加速但速度相对较慢来判断角色是否撞到了墙壁。

	float Dot = UKismetMathLibrary::Dot_VectorVector(
		UKismetMathLibrary::Normal(LocomotionData.Movement.LocalAcceleration2D),
		UKismetMathLibrary::Normal(LocomotionData.Movement.LocalVelocity2D));
	LocomotionData.States.IsRunningIntoWall = UKismetMathLibrary::VSizeXY(LocomotionData.Movement.LocalVelocity2D) > 0.1
		&&
		UKismetMathLibrary::VSizeXY(LocomotionData.Movement.LocalAcceleration2D) > 0.1 &&
		UKismetMathLibrary::InRange_FloatFloat(Dot, -0.6, 0.6, true, true);
#pragma endregion

#pragma region 布娃娃更新
	LocomotionData.Ragdoll.bIsRagdoll = GetCancerCharacterMovementComponent()->bIsRagdollState;
	if (LocomotionData.Ragdoll.bIsRagdoll)
	{
		const float VelocityLength = GetOwningComponent()->GetPhysicsLinearVelocity("root").Size();
		LocomotionData.Ragdoll.FlailRate = FMath::GetMappedRangeValueClamped<float, float>
			({0.0f, 1000.0f}, {0.0f, 1.0f}, VelocityLength);
	}

#pragma endregion

#pragma region  计算倾斜


	if (LocomotionData.States.bHasAcceleration && LocomotionData.States.bHasVelocity ||
		LocomotionData.Movement.Velocity2D.Size() > 150)
	{
		if (!LocomotionData.States.IsOnGround)
		{
			// Interp and set the In Air Lean Amount
			const FCancerLeanAmount& InAirLeanAmount = CalculateAirLeanAmount();
			LocomotionData.LeanAmount.LeanAmount.LR = FMath::FInterpTo(LocomotionData.LeanAmount.LeanAmount.LR,
			                                                           InAirLeanAmount.LR, DeltaSeconds,
			                                                           LocomotionConfig.GroundedLeanInterpSpeed);
			LocomotionData.LeanAmount.LeanAmount.FB = FMath::FInterpTo(LocomotionData.LeanAmount.LeanAmount.FB,
			                                                           InAirLeanAmount.FB, DeltaSeconds,
			                                                           LocomotionConfig.GroundedLeanInterpSpeed);
		}
		else
		{
			LocomotionData.Movement.VelocityAcceleration = GetCancerCharacterMovementComponent()->VelocityAcceleration;

			//Set the Relative Acceleration Amount and Interp the Lean Amount.
			//设置相对加速度 并计算倾斜
			LocomotionData.LeanAmount.RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
			LocomotionData.LeanAmount.LeanAmount.LR = FMath::FInterpTo(LocomotionData.LeanAmount.LeanAmount.LR,
			                                                           LocomotionData.LeanAmount.
			                                                           RelativeAccelerationAmount.Y, DeltaSeconds,
			                                                           LocomotionConfig.GroundedLeanInterpSpeed);
			LocomotionData.LeanAmount.LeanAmount.FB = FMath::FInterpTo(LocomotionData.LeanAmount.LeanAmount.FB,
			                                                           LocomotionData.LeanAmount.
			                                                           RelativeAccelerationAmount.X, DeltaSeconds,
			                                                           LocomotionConfig.GroundedLeanInterpSpeed);
		}
	}
#pragma endregion

	LocomotionData.bFirstUpdate = false;
}

UCharacterMovementComponent* UCancerAnimInstance::GetCharacterMovementComponent() const
{
	if (UCharacterMovementComponent* CharacterMovementComponent = CharacterPtr->GetCharacterMovement())
	{
		return CharacterMovementComponent;
	}
	return nullptr;
}

UCancerMovementComponent* UCancerAnimInstance::GetCancerCharacterMovementComponent() const
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovementComponent())
	{
		return Cast<UCancerMovementComponent>(CharacterMovementComponent);
	}
	return nullptr;
}

void UCancerAnimInstance::SetCustomMovementMode(ECustomRotationMode CustomMode)
{
	if (GetCancerCharacterMovementComponent())
		GetCancerCharacterMovementComponent()->CustomRotationData.CustomRotationMode = CustomMode;
}

void UCancerAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);
}

#if WITH_EDITOR
EDataValidationResult UCancerAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif

FVector UCancerAnimInstance::CalculateRelativeAccelerationAmount() const
{
	if (GetCharacterMovementComponent()->MaxAcceleration > 0 &&
		GetCharacterMovementComponent()->GetMaxBrakingDeceleration() > 0)
	{
		if (UKismetMathLibrary::Dot_VectorVector(LocomotionData.Movement.Velocity2D,
		                                         LocomotionData.Movement.Acceleration2D) > 0)
		{
			// 加速：按最大加速度归一化
			FVector Temp = UKismetMathLibrary::Vector_ClampSizeMax(
				LocomotionData.Movement.VelocityAcceleration,
				GetCharacterMovementComponent()->MaxAcceleration
			) / GetCharacterMovementComponent()->MaxAcceleration;

			return UKismetMathLibrary::LessLess_VectorRotator(Temp, LocomotionData.Rotation.ActorRotation);
		}
		else
		{
			// 减速：按最大刹车减速度归一化，并返回旋转后的向量（之前没有返回导致总是零）
			FVector Temp = UKismetMathLibrary::Vector_ClampSizeMax(
				LocomotionData.Movement.VelocityAcceleration,
				GetCharacterMovementComponent()->GetMaxBrakingDeceleration()
			) / GetCharacterMovementComponent()->GetMaxBrakingDeceleration();

			return UKismetMathLibrary::LessLess_VectorRotator(Temp, LocomotionData.Rotation.ActorRotation);
		}
	}
	return FVector::ZeroVector;
}

FCancerLeanAmount UCancerAnimInstance::CalculateAirLeanAmount() const
{
	// Use the relative Velocity direction and amount to determine how much the character should lean while in air.
	// The Lean In Air curve gets the Fall Speed and is used as a multiplier to smoothly reverse the leaning direction
	// when transitioning from moving upwards to moving downwards.
	FCancerLeanAmount CalcLeanAmount;
	const FVector& UnrotatedVel = LocomotionData.Rotation.ActorRotation.UnrotateVector(
		GetCharacterMovementComponent()->Velocity) / 350.0f;

	FVector2D InversedVect(UnrotatedVel.Y, UnrotatedVel.X);
	if (LocomotionConfig.LeanInAirCurve)
		InversedVect *= LocomotionConfig.LeanInAirCurve->GetFloatValue(GetCharacterMovementComponent()->Velocity.Z);
	CalcLeanAmount.LR = InversedVect.X;
	CalcLeanAmount.FB = InversedVect.Y;
	return CalcLeanAmount;
}

void UCancerAnimInstance::SetDoubleJump(bool InDoubleJump)
{
	LocomotionData.States.bIsDoubleJump = InDoubleJump;
}

void UCancerAnimInstance::SavePose(const FName SnapshotName)
{
	SavePoseSnapshot(SnapshotName);
}

void UCancerAnimInstance::SetRootYawOffset(float InRootYawOffset)
{
	if (LocomotionConfig.bEnableRootYawOffset)
	{
		FVector2D NewRootYawOffsetAngleClamp = LocomotionData.States.bStand
			                                       ? RootYawOffsetAngleClamp
			                                       : RootYawOffsetAngleClampCrouched;

		float TempAngle = UKismetMathLibrary::ClampAngle(UKismetMathLibrary::NormalizeAxis(InRootYawOffset),
		                                                 NewRootYawOffsetAngleClamp.X,
		                                                 NewRootYawOffsetAngleClamp.Y);
		if (NewRootYawOffsetAngleClamp.X == NewRootYawOffsetAngleClamp.Y)
		{
			LocomotionData.Rotation.RootYawOffset = InRootYawOffset;
		}
		else
		{
			LocomotionData.Rotation.RootYawOffset = TempAngle;
		}
		LocomotionData.Rotation.AimYaw = LocomotionData.Rotation.RootYawOffset * -1.f;
	}
	else
	{
		LocomotionData.Rotation.RootYawOffset = 0.f;
		LocomotionData.Rotation.AimYaw = 0.f;
	}
}
