#include "Components/CancerMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "CancerAssetManager.h"
#include "KismetTraceUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/MovementDefinition.h"

DEFINE_LOG_CATEGORY(CancerMovementLog)

static float GroundTraceDistance = 100000.0f;

static TAutoConsoleVariable<bool> RagdollVar(TEXT("Cancer.DebugRagdoll"), false,TEXT(""));
static TAutoConsoleVariable<bool> TraceLedgeVar(TEXT("Cancer.TraceLedge"), false,TEXT(""));
static TAutoConsoleVariable<bool> CVarDebugGroundInfo(TEXT("Cancer.DebugGroundInfo"), false, TEXT("Show ground distance debug info"));

namespace CancerMovement
{
	FString GetTagLatName(const FString& InName)
	{
		TArray<FString> Names;
		InName.ParseIntoArray(Names,TEXT("."), true);
		return Names.Last();
	}
}

UCancerMovementComponent::UCancerMovementComponent(const FObjectInitializer& InitializerModule)
{
	BrakingFrictionFactor = 1;
	MaxAcceleration = 2400;
	SetCrouchedHalfHeight(65);
	BrakingFriction = 6;
	GroundFriction = 8;
	bCanWalkOffLedgesWhenCrouching = true;
	BrakingDecelerationWalking = 1400;
	NavMovementProperties.bUseAccelerationForPaths = true;
	NavAgentProps.bCanCrouch = true;
}

void UCancerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	SetOrientToMovement(true);
}

void UCancerMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UCancerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetEssentialValues(DeltaTime);
	SetPreviousValues();
	UpdateMovementParameter();

	if (bIsRagdollState)
		RagdollUpdate(DeltaTime);
}

void UCancerMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	// 非锁定移动模式下 才能切换移动模式
	if (!bMovementModeLocked)
	{
		Super::SetMovementMode(NewMovementMode, NewCustomMode);
	}
}

FVector UCancerMovementComponent::ConsumeInputVector()
{
	FVector InputVector{Super::ConsumeInputVector()};

	//阻挡移动输入
	if (bInputBlocked)
	{
		return FVector::ZeroVector;
	}

	// 边缘防走落：仅在地面行走时移除“朝边缘前进”的分量，空中不拦截输入
	if (bEnableTraceLedgesCheck && CharacterOwner && IsMovingOnGround())
	{
		const FVector Input2D(InputVector.X, InputVector.Y, 0.f);
		if (!Input2D.IsNearlyZero())
		{
			const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
			const float CapsuleRadius = CapsuleComp ? CapsuleComp->GetUnscaledCapsuleRadius() : 34.f;
			const FVector ForwardDir = Input2D.GetSafeNormal();
			const FVector CapsuleLocation = CharacterOwner->GetActorLocation();
			const FVector Down = FVector::DownVector * (WalkOffLedgesHeight + 90.f);
			const float TraceRadius = FMath::Max(10.f, CapsuleRadius * 0.5f);
			const float EdgeStop = CapsuleRadius + WalkOffLedgesDis;
			const FVector Location = CapsuleLocation + ForwardDir * EdgeStop;

			const int32 Count = 5;
			const float HalfAngle = 15.f;
			const FRotator ForwardRot = ForwardDir.Rotation();

			bool bHasGround = true;
			for (int32 i = 0; i < Count; ++i)
			{
				float Alpha = static_cast<float>(i) / static_cast<float>(Count);
				Alpha = Alpha * 2.f - 1.f;
				const float v = Alpha * UE_PI * (HalfAngle * 2.f / 180.f);

				FVector Offset = FVector(FMath::Cos(v), FMath::Sin(v), 0.f);
				Offset = ForwardRot.RotateVector(Offset);
				Offset *= CapsuleRadius + TraceRadius;

				const FVector a = Location + Offset;
				const FVector b = Location + Down + Offset;

				FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, CharacterOwner);
				FCollisionResponseParams ResponseParam;
				InitCollisionParams(QueryParams, ResponseParam);
				const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

				FHitResult Hit(1.f);
				const bool bHit = GetWorld()->SweepSingleByChannel(Hit, a, b, FQuat::Identity, CollisionChannel,
				                                                   FCollisionShape::MakeSphere(TraceRadius),
				                                                   QueryParams, ResponseParam);
				if (!bHit)
				{
					bHasGround = false;
					break;
				}
			}

			if (!bHasGround)
			{
				// 保留沿边方向的输入，去除朝边缘前进的分量
				const FVector SideDir = FVector::CrossProduct(FVector::UpVector, ForwardDir).GetSafeNormal();
				const float SideMag = FVector::DotProduct(Input2D, SideDir);
				InputVector = SideDir * SideMag;
				// 维持水平输入，避免引入 Z 分量
				InputVector.Z = 0.f;
			}
		}
	}

	return InputVector;
}

void UCancerMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
	//速度或者加速度相反 触发折返
	if ((GetCurrentAcceleration().GetSafeNormal() | Velocity.GetSafeNormal()) < 0.f)
	{
		BP_Pivot();
	}
}

void UCancerMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult,
                                         bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
{
	Super::FindFloor(CapsuleLocation, OutFloorResult, bCanUseCachedLocation, DownwardSweepResult);

	if (OutFloorResult.bWalkableFloor && bEnableTraceLedgesCheck && Velocity.Size2D() > 10.f)
	{
		const bool bHasGroundAhead = TraceLedges(CapsuleLocation);
		if (!bHasGroundAhead)
		{
			OutFloorResult.bWalkableFloor = false;
		}
	}
}

bool UCancerMovementComponent::CanWalkOffLedges() const
{
	//return Super::CanWalkOffLedges();
	return TraceLedges(GetActorLocation());
}

void UCancerMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bUseCustomRotation)
	{
		const FRotator& CurrentRotation = UpdatedComponent->GetComponentRotation();

		FRotator DesiredRotation = CurrentRotation;

		//DesiredRotation.Yaw = FRotator::NormalizeAxis(LockOnRotationYaw);

		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, 1E-03))
		{
			DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw,
			                                       GetDeltaRotation(DeltaTime).Yaw);
		}
		MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation.GetDenormalized(), false);
	}
	else if (CustomRotationData.bUseCustomRotation)
	{
		if (!(bOrientRotationToMovement || bUseControllerDesiredRotation))
			return;

		// 获取当前组件的旋转
		const FRotator& CurrentRotation = UpdatedComponent->GetComponentRotation();

		FRotator DesiredRotation = CurrentRotation;

		// 将期望旋转的偏航角设置为自定义旋转角度（0~±180）
		DesiredRotation.Yaw = FRotator::NormalizeAxis(CustomRotationData.CustomRotationYaw);

		// 如果当前偏航角与期望偏航角有显著差异（超过0.001度）
		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, 1E-03))
		{
			// 平滑过渡到目标角度
			// GetDeltaRotation：返回在 DeltaTime 时间间隔内需要旋转角色的角度
			DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw,
			                                       GetDeltaRotation(DeltaTime).Yaw);
		}
		// 应用旋转变化：移动量为零，只更新旋转，false 表示不启用扫描检测（防止碰撞检测）
		MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation.GetDenormalized(), false);
	}
	else
	{
		Super::PhysicsRotation(DeltaTime);
	}
}

FRotator UCancerMovementComponent::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime,
                                                                   FRotator& DeltaRotation) const
{
	if (Acceleration.SizeSquared() < UE_KINDA_SMALL_NUMBER)
	{
		// AI path following request can orient us in that direction (it's effectively an acceleration)
		if (bHasRequestedVelocity && RequestedVelocity.SizeSquared() > UE_KINDA_SMALL_NUMBER)
		{
			return RequestedVelocity.GetSafeNormal().Rotation();
		}

		// Don't change rotation if there is no acceleration.
		return CurrentRotation;
	}


	/*if(CurGaitTag == GaitTag::Sprint )
		{
			FVector2d SprintStartVector = TestCharacter->SprintStartInputValue;
			FVector2D LastMoveInputValue = TestCharacter->LastMoveInputValue;
			if (SprintStartVector == LastMoveInputValue)
			{
				return CurrentRotation;
			}
			FRotator Rotator = FQuat::FindBetween(FVector(SprintStartVector.Y, SprintStartVector.X, 0), FVector(LastMoveInputValue.Y, LastMoveInputValue.X, 0)).Rotator();
			return TestCharacter->SprintStartRotation + FRotator(0, Rotator.Yaw, 0);
		}*/
	return Super::ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRotation);
}

FRotator UCancerMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	/*if (const ACharacter* OwnerCharacter = Cast<ACharacter>(GetCharacterOwner()))
	{
		const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
		const FVector GVector = GetLastInputVector();
		const FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(GVector);
		float DesiredRotationYaw = (TargetRotation - CurrentRotation).Yaw;
		if (abs(DesiredRotationYaw) > 180)
		{
			if (TargetRotation.Yaw > CurrentRotation.Yaw)
			{
				DesiredRotationYaw = DesiredRotationYaw - 360;
			}d
			else
			{
				DesiredRotationYaw = 360 + DesiredRotationYaw;
			}
		}
		return FRotator(0, DesiredRotationYaw, 0) * GlobalRotationRate * DeltaTime;
	}*/

	return Super::GetDeltaRotation(DeltaTime);
}

bool UCancerMovementComponent::TraceLedges(const FVector& CapsuleLocation) const
{
	if (!bEnableTraceLedgesCheck)
	{
		return true;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	const UCapsuleComponent* CapsuleComp = CharacterOwner ? CharacterOwner->GetCapsuleComponent() : nullptr;
	const float CapsuleRadius = CapsuleComp ? CapsuleComp->GetUnscaledCapsuleRadius() : 34.f;
	const FVector Down = FVector::DownVector * (WalkOffLedgesHeight + 90.f);
	FVector ForwardDir = Velocity.Size2D() > 1.f ? Velocity.GetSafeNormal2D() : FVector::ZeroVector;
	if (ForwardDir.IsNearlyZero())
	{
		const FVector LastInput = GetLastInputVector();
		ForwardDir = FVector(LastInput.X, LastInput.Y, 0.f).GetSafeNormal();
	}
	if (ForwardDir.IsNearlyZero())
	{
		// 没有有效的前进方向，不阻挡（例如纯旋转或静止）
		return true;
	}
	const FRotator ForwardRot = ForwardDir.Rotation();
	const float HalfAngle = 15.f;
	const float TraceRadius = FMath::Max(10.f, CapsuleRadius * 0.5f);
	const int32 Count = 5;

	// 从胶囊体前缘再向前 EdgeStop 距离处进行向下扫描，确保前方仍有地面
	const float EdgeStop = CapsuleRadius + WalkOffLedgesDis;
	const FVector Location = CapsuleLocation + ForwardDir * EdgeStop;

	for (int32 i = 0; i < Count; ++i)
	{
		float Alpha = static_cast<float>(i) / static_cast<float>(Count);
		Alpha = Alpha * 2.f - 1.f;

		float v = Alpha * UE_PI * (HalfAngle * 2.f / 180.f);

		FVector Offset = FVector::ZeroVector;
		Offset.X = FMath::Cos(v);
		Offset.Y = FMath::Sin(v);

		Offset = ForwardRot.RotateVector(Offset);

		// 以胶囊半径为基准的横向采样半径，覆盖前缘两侧
		Offset *= CapsuleRadius + TraceRadius;

		const FVector a = Location + Offset;
		const FVector b = Location + Down + Offset;

		FHitResult Hit(1.f);
		const bool bHit = GetWorld()->SweepSingleByChannel(Hit, a, b, FQuat::Identity, CollisionChannel,
		                                                   FCollisionShape::MakeSphere(TraceRadius),
		                                                   QueryParams, ResponseParam);
#if WITH_EDITOR
		if (TraceLedgeVar.GetValueOnGameThread())
		{
			DrawDebugSphereTraceSingle(GetWorld(), a, b, TraceRadius, EDrawDebugTrace::ForOneFrame, bHit, Hit,
			                           FLinearColor::Red, FLinearColor::Green,
			                           1.f);
		}
#endif

		if (!bHit)
		{
			return false;
		}
	}
	return true;
}

void UCancerMovementComponent::DrawDebugMovementInfo()
{
	if (!GetOwner()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector ActorLocation = GetOwner()->GetActorLocation();
	FVector BaseLocation = ActorLocation - FVector(0, 0, GetOwner()->GetSimpleCollisionHalfHeight());

	// 获取速度和加速度
	FVector Accel = GetCurrentAcceleration();

	// 计算箭头长度映射 (速度和加速度)
	float Speed = Velocity.Size();
	float MaxSpeed = GetMaxSpeed(); // 最大速度，用于归一化
	float SpeedAlpha = FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f);
	float SpeedArrowLength = FMath::Lerp(50.f, 100.f, SpeedAlpha);

	float AccelMag = Accel.Size();
	float AccelAlpha = FMath::Clamp(AccelMag / MaxSpeed, 0.f, 1.f);
	float AccelArrowLength = FMath::Lerp(50.f, 100.f, AccelAlpha);

	// 绘制速度箭头 (绿色)
	if (!Velocity.IsNearlyZero())
	{
		FVector EndPos = BaseLocation + Velocity.GetSafeNormal() * SpeedArrowLength;
		DrawDebugDirectionalArrow(World, BaseLocation, EndPos, 20.f, FColor::Green, false, -1.f, 0, 2.f);
		DrawDebugString(World, EndPos, TEXT("Speed"), nullptr, FColor::Green, 0.f, true);
	}

	// 绘制加速度箭头 (红色)
	if (!Accel.IsNearlyZero())
	{
		FVector EndPos = BaseLocation + Accel.GetSafeNormal() * AccelArrowLength;
		DrawDebugDirectionalArrow(World, BaseLocation, EndPos, 20.f, FColor::Red, false, -1.f, 0, 2.f);
		DrawDebugString(World, EndPos, TEXT("Accel"), nullptr, FColor::Red, 0.f, true);
	}

	// 绘制圆 (基于角色前方向)
	FVector Forward = GetOwner()->GetActorForwardVector();
	FVector Right = GetOwner()->GetActorRightVector();

	float Radius = 150.f;
	int32 Segments = 64;
	DrawDebugCircle(World, BaseLocation, Radius, Segments, FColor::Blue, false, -1.f, 0, 2.f, Forward, Right, false);

	// 画12个刻度
	for (int32 i = 0; i < 12; ++i)
	{
		float AngleDeg = i * 30.f; // 每个刻度 30度
		float AngleRad = FMath::DegreesToRadians(AngleDeg);

		FVector Dir = Forward.RotateAngleAxis(AngleDeg, FVector::UpVector);
		FVector TickStart = BaseLocation + Dir * (Radius - 10.f);
		FVector TickEnd = BaseLocation + Dir * (Radius + 10.f);

		DrawDebugLine(World, TickStart, TickEnd, FColor::Yellow, false, -1.f, 0, 2.f);

		// 显示角度文字
		FVector TextPos = BaseLocation + Dir * (Radius + 20.f);
		FString AngleText = FString::Printf(TEXT("%.0f"), AngleDeg);
		DrawDebugString(World, TextPos, AngleText, nullptr, FColor::White, 0.f, true);
	}
}

void UCancerMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	UpdateCustomRotation(DeltaSeconds); //更新自定义旋转
}

void UCancerMovementComponent::UpdateCustomRotation(float DeltaTime)
{
	if (!GetCharacterOwner()->GetMesh()->GetAnimInstance()) return;

	CustomRotationData.ControlRotation = GetCharacterOwner()->GetController()->GetDesiredRotation().GetNormalized();

	CustomRotationData.CustomRotationCurveWeight = GetCharacterOwner()->GetMesh()->GetAnimInstance()->GetCurveValue(
		"Enable_CustomRotation");
	/*启用自定义旋转的条件：
	1. 没有根运动源（根运动优先）不能妨碍攻击、交互等蒙太奇的跟运动
	2. 自定义旋转曲线权重大于0.99（几乎完全启用）*/
	CustomRotationData.bUseCustomRotation = !HasRootMotionSources() && CustomRotationData.CustomRotationCurveWeight >
		0.99f;

	// 如果当前角色正在移动: 有加速度不一定有速度(被挡住时)；有速度肯定是在移动的，但不一定有加速度(惯性导致的移动)
	if (GetCurrentAcceleration().SizeSquared() > KINDA_SMALL_NUMBER && Velocity.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		// 计算运动中加速度方向的旋转（归一化加速度向量的旋转）
		CustomRotationData.AccelerationRotation = GetCurrentAcceleration().GetSafeNormal().Rotation();
	}

	// 如果启用了自定义旋转
	if (CustomRotationData.bUseCustomRotation)
	{
		// 根据不同的自定义旋转模式处理旋转
		switch (CustomRotationData.CustomRotationMode)
		{
		case ECustomRotationMode::EHold:
			{
				float DesiredYaw = UpdatedComponent->GetComponentRotation().GetNormalized().Yaw;
				CustomRotationData.CustomRotationYaw = !bUseControllerDesiredRotation
					                                       ? DesiredYaw
					                                       : CharacterOwner->GetController()->GetDesiredRotation().Yaw;
				break;
			}
		case ECustomRotationMode::EInterpolation:
			{
				// 插值模式：如果有加速度，平滑过渡到加速度方向
				const FRotator& CurrentRotation = UpdatedComponent->GetComponentRotation().GetNormalized();
				const FRotator& DesiredRotation = FMath::RInterpTo(CurrentRotation,
				                                                   CustomRotationData.AccelerationRotation, DeltaTime,
				                                                   10.f);
				CustomRotationData.CustomRotationYaw = !bUseControllerDesiredRotation
					                                       ? DesiredRotation.Yaw
					                                       : CharacterOwner->GetController()->GetDesiredRotation().Yaw;
				break;
			}
		case ECustomRotationMode::EAnimRotation:
			{
				// 动画旋转模式：由动画系统完全控制，这里不做处理
				break;
			}
		}
	}
	else
	{
		// 未启用自定义旋转时，使用当前组件旋转
		CustomRotationData.CustomRotationYaw = UpdatedComponent->GetComponentRotation().GetNormalized().Yaw;
	}

	// 确保自定义旋转角度在合法的0-360度范围内
	CustomRotationData.CustomRotationYaw = FRotator::ClampAxis(CustomRotationData.CustomRotationYaw);
}

void UCancerMovementComponent::ApplyRootMotionToVelocity(float DeltaTime)
{
	FVector PreRootMotionVelocity = Velocity; // 记录应用根运动前的速度

	Super::ApplyRootMotionToVelocity(DeltaTime);

	if (!HasAnimRootMotion())
	{
		return;
	}
	FVector RootMotionVelocity = RootMotionParams.GetRootMotionTransform().GetTranslation() / DeltaTime;
	FVector FinalVelocity = RootMotionVelocity;
	if (!bAllowHorizontalRootMotion)
	{
		// 屏蔽 XY：保留物理系统的水平速度，不使用动画位移
		FinalVelocity.X = PreRootMotionVelocity.X;
		FinalVelocity.Y = PreRootMotionVelocity.Y;
	}
	if (!bAllowVerticalRootMotion)
	{
		// 屏蔽 Z：让重力或物理系统的 Z 轴速度接管，动画不再控制高度
		FinalVelocity.Z = PreRootMotionVelocity.Z;
	}
	Velocity = FinalVelocity;
	Velocity *= RootMotionScale;
}

bool UCancerMovementComponent::IsSprint() const
{
	return CurGaitTag == GaitTag::Sprint;
}

bool UCancerMovementComponent::IsDesiredSprint() const
{
	return DesiredGaitTag == GaitTag::Sprint;
}

void UCancerMovementComponent::SetStance(bool NewStand)
{
	if (bStand != NewStand)
	{
		bStand = NewStand;
		if (bStand)
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}
	}
}

void UCancerMovementComponent::SetDesiredGait(FGameplayTag NewTag)
{
	DesiredGaitTag = NewTag;
}

void UCancerMovementComponent::SetOrientToMovement(bool NewRotationMode)
{
	bOrientToMovement = NewRotationMode;
	if (bOrientToMovement)
	{
		//速度移动
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
	}
	else
	{
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
	}
}

bool UCancerMovementComponent::CanSprint() const
{
	//只有满足站立冲刺
	if (!bStand)
		return false;
	return true;
}

FGameplayTag UCancerMovementComponent::GetAllowedGait() const
{
	//站立
	if (bStand)
	{
		if (bOrientToMovement)
		{
			if (DesiredGaitTag == GaitTag::Sprint)
			{
				if (CanSprint())
				{
					return GaitTag::Sprint;
				}
				return GaitTag::Run;
			}
			else
			{
				return DesiredGaitTag;
			}
		}
		else
		{
			if (DesiredGaitTag == GaitTag::Sprint)
			{
				if (CanSprint())
				{
					return GaitTag::Sprint;
				}
				return GaitTag::Run;
			}
			else
			{
				return DesiredGaitTag;
			}
		}
	}
	//下蹲
	else
	{
		if (bOrientToMovement)
		{
			if (DesiredGaitTag == GaitTag::Sprint)
			{
				return GaitTag::Run;
			}
			else
			{
				return DesiredGaitTag;
			}
		}
		else
		{
			if (DesiredGaitTag == GaitTag::Sprint)
			{
				return GaitTag::Run;
			}
			else
			{
				return DesiredGaitTag;
			}
		}
	}
}

FGameplayTag UCancerMovementComponent::GetActualGait(const FGameplayTag& AllowTag) const
{
	FString Str = bOrientToMovement ? TEXT("Movement.OrientToMovement") : TEXT("Movement.Strafe");
	FName InNameWalk = *(Str + "." + "Walk");
	FName InNameRun = *(Str + "." + "Run");

	UCancerMovementNode* WalkNode = GetMovementNode(InNameWalk);
	UCancerMovementNode* RunNode = GetMovementNode(InNameRun);

	if (!WalkNode || !RunNode) return GaitTag::Run;
	const float LocWalkSpeed = WalkNode->GetMaxWalkSpeed();
	const float LocRunSpeed = RunNode->GetMaxWalkSpeed();

	float CurSpeed = UKismetMathLibrary::VSizeXY(Velocity);

	if (CurSpeed >= (LocRunSpeed + 10))
	{
		if (AllowTag == GaitTag::Walk)
		{
			return GaitTag::Run;
		}
		else if (AllowTag == GaitTag::Run)
		{
			return GaitTag::Run;
		}
		else
		{
			return GaitTag::Sprint;
		}
	}
	else
	{
		if (CurSpeed >= (LocWalkSpeed + 10))
		{
			return GaitTag::Run;
		}
		else
		{
			return GaitTag::Walk;
		}
	}
}


float UCancerMovementComponent::GetMovmentPrarameterSpeed(FGameplayTag IntGaitTag) const
{
	FString Str = bOrientToMovement ? TEXT("Movement.OrientToMovement") : TEXT("Movement.Strafe");
	FString Str1 = CancerMovement::GetTagLatName(IntGaitTag.ToString());
	FName InName = *(Str + "." + Str1);

	if (UCancerMovementNode* Node = GetMovementNode(InName))
	{
		return Node->GetMaxWalkSpeed();
	}
	return 0.0f;
}

void UCancerMovementComponent::SetMovmentPrarameterSpeed(FGameplayTag IntGaitTag, float Speed) const
{
	FString Str = bOrientToMovement ? TEXT("Movement.OrientToMovement") : TEXT("Movement.Strafe");
	FString Str1 = CancerMovement::GetTagLatName(IntGaitTag.ToString());
	FName InName = *(Str + "." + Str1);

	if (UCancerMovementNode* Node = GetMovementNode(InName))
	{
		Node->SetMaxWalkSpeed(Speed);
	}
}


UCancerMovementNode* UCancerMovementComponent::GetMovementNode(const FName& InName) const
{
	if (MovementParameter)
		return MovementParameter->FindNodeByName(InName);
	return nullptr;
}

void UCancerMovementComponent::UpdateMovementParameter()
{
	FString Str = bOrientToMovement ? TEXT("Movement.OrientToMovement") : TEXT("Movement.Strafe");
	FString Str1 = CancerMovement::GetTagLatName(DesiredGaitTag.ToString());
	FName InName = *(Str + "." + Str1);
	UCancerMovementNode* Node = GetMovementNode(InName);
	if (Node != nullptr)
	{
		Node->PreUpdateMovementParameter(this);
		Node->UpdateMovementRotation(this, bUseCurveRotation);
	}
	else
	{
		UE_LOG(CancerMovementLog, Error, TEXT("没有找到为：%s 的节点"), *InName.ToString());
	}
	FGameplayTag AllowedGait = GetAllowedGait();
	//TODO::
	/*FGameplayTag ActualGait = GetActualGait(AllowedGait);
	if (CurGaitTag != ActualGait)
	{
		CurGaitTag = ActualGait;
		Node->PostUpdateMovementParameter(this);
	}*/
	CurGaitTag = AllowedGait;
	Node->PostUpdateMovementParameter(this);
}


void UCancerMovementComponent::BP_Pivot_Implementation()
{
}

const FCancerCharacterGroundInfo& UCancerMovementComponent::GetGroundInfo()
{
	// 如果没有角色所有者，或者缓存信息是当前帧的，直接返回缓存
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}
	// 如果角色已经在地面上
	if (MovementMode == MOVE_Walking)
	{
		// 直接使用当前的地面信息
		// CurrentFloor.HitResult: 地面碰撞结果
		// GroundDistance: 地面距离（已知在地面上，设置为0）
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);


		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();

		// 设置射线检测的碰撞通道
		// 如果UpdatedComponent存在，使用其碰撞类型，否则使用默认的Pawn碰撞类型
		const ECollisionChannel CollisionChannel = (UpdatedComponent
			                                            ? UpdatedComponent->GetCollisionObjectType()
			                                            : ECC_Pawn);

		// 计算射线起点（角色位置）
		const FVector TraceStart(GetActorLocation());

		// 计算射线终点（向下GroundTraceDistance距离，减去胶囊体半高）
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - GroundTraceDistance - CapsuleHalfHeight));

		// 设置碰撞查询参数
		// SCENE_QUERY_STAT: 添加性能统计
		// false: 不显示调试信息
		// CharacterOwner: 忽略自身碰撞
		FCollisionQueryParams QueryParams(
			SCENE_QUERY_STAT(CancerCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams,
		                                     ResponseParam);

		// 更新地面信息
		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = GroundTraceDistance;

		// 可视化调试
		if (IsInGameThread() && CVarDebugGroundInfo.GetValueOnGameThread())
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, -1, 0, 1.0f);
			if (HitResult.bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 8, FColor::Red, false, -1, 0, 1.0f);
			}
		}
		// 根据运动模式更新地面距离
		if (MovementMode == MOVE_NavWalking)
		{
			// 导航行走模式，地面距离设为0
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			// 如果检测到地面碰撞
			// 计算实际地面距离（减去胶囊体半高，确保不会小于0）
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}
	// 更新缓存信息的最后更新帧
	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

void UCancerMovementComponent::SetEssentialValues(float DeltaTime)
{
	VelocityAcceleration = (Velocity - PreviousVelocity) / DeltaTime;

	CurrentSpeed2D = UKismetMathLibrary::VSizeXY(Velocity);

	bIsMove = CurrentSpeed2D > 1.0f;
	if (bIsMove)
	{
		LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(Velocity);
	}

	float MovementInputAmount = GetCurrentAcceleration().Length() / GetMaxAcceleration();
	HasMovementInput = MovementInputAmount > 0.0f;
	if (HasMovementInput)
	{
		LastMovementInputRotation = UKismetMathLibrary::Conv_VectorToRotator(GetCurrentAcceleration());
	}
	else
	{
		//TODO::
		//BP_TurnStart();
	}

	CurrentAimRotationYaw = UKismetMathLibrary::Abs((GetCharacterOwner()->GetControlRotation().Yaw
		- PreviousRotationYaw) / DeltaTime);
}

void UCancerMovementComponent::SetPreviousValues()
{
	PreviousVelocity = Velocity;
	PreviousRotationYaw = GetCharacterOwner()->GetControlRotation().Yaw;
}

#pragma region 布娃娃
static FName RagdollName = "pelvis";
static FName RootName = "root";

UAnimMontage* UCancerMovementComponent::GetRagdollMontage(bool InRagdollFaceUp) const
{
	TSoftObjectPtr<UAnimMontage> Montage = InRagdollFaceUp ? GetUp_F : GetUp_B;

	return UCancerAssetManager::Get().GetAsset(Montage);
}

FName UCancerMovementComponent::GetRagdollPoseSnapshotName() const
{
	return FName("RagdollPose");
}

void UCancerMovementComponent::RagdollStart()
{
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(true);
	}
	bIsRagdollState = true;

	// Step 1: Clear the Character Movement Mode and set the Movement State to Ragdoll
	SetMovementMode(MOVE_None);

	UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
	USkeletalMeshComponent* MeshComp = CharacterOwner->GetMesh();

	// Step 2: Disable capsule collision and enable mesh physics simulation starting from the pelvis.
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetAllBodiesBelowSimulatePhysics(RagdollName, true, true);

	// Step 3: Stop any active montages.
	if (MeshComp->GetAnimInstance())
	{
		MeshComp->GetAnimInstance()->Montage_Stop(0.2f);
	}
}

void UCancerMovementComponent::RagdollEnd()
{
	UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
	USkeletalMeshComponent* MeshComp = CharacterOwner->GetMesh();

	// Step 1: Save a snapshot of the current Ragdoll Pose for use in AnimGraph to blend out of the ragdoll
	if (MeshComp->GetAnimInstance())
	{
		MeshComp->GetAnimInstance()->SavePoseSnapshot(GetRagdollPoseSnapshotName());
	}

	// Step 2: If the ragdoll is on the ground, set the movement mode to walking and play a Get Up animation.
	// If not, set the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.
	if (bRagdollOnGround)
	{
		SetMovementMode(MOVE_Walking);
		if (MeshComp->GetAnimInstance())
		{
			MeshComp->GetAnimInstance()->Montage_Play(GetRagdollMontage(bRagdollFaceUp), 1.0f,
			                                          EMontagePlayReturnType::MontageLength, 0.0f, true);
		}
	}
	else
	{
		SetMovementMode(MOVE_Falling);
		Velocity = LastRagdollVelocity;
	}

	// Step 3: Re-Enable capsule collision, and disable physics simulation on the mesh.
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_Pawn);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetAllBodiesSimulatePhysics(false);

	bIsRagdollState = false;
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(false);
	}
}

void UCancerMovementComponent::RagdollUpdate(float DeltaTime)
{
	USkeletalMeshComponent* MeshComp = CharacterOwner->GetMesh();

	// Set the Last Ragdoll Velocity.
	const FVector NewRagdollVel = MeshComp->GetPhysicsLinearVelocity(RootName);
	LastRagdollVelocity = (NewRagdollVel != FVector::ZeroVector)
		                      ? NewRagdollVel
		                      : LastRagdollVelocity / 2;

	// Use the Ragdoll Velocity to scale the ragdoll's joint strength for physical animation.
	const float SpringValue = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 1000.0f}, {0.0f, 25000.0f},
	                                                                          LastRagdollVelocity.Size());
	MeshComp->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual acceleration.
	// This also prevents the ragdoll from going through the floor.
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	MeshComp->SetEnableGravity(bEnableGrav);

	SetActorLocationDuringRagdoll(DeltaTime);
}

void UCancerMovementComponent::SetActorLocationDuringRagdoll(float DeltaTime)
{
	UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
	USkeletalMeshComponent* MeshComp = CharacterOwner->GetMesh();

	FVector TargetRagdollLocation = MeshComp->GetSocketLocation(RagdollName);
	FRotator RagdollRotation = MeshComp->GetSocketRotation(RagdollName);

	bRagdollFaceUp = RagdollRotation.Roll < 0;
	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? RagdollRotation.Yaw - 180.0f : RagdollRotation.Yaw,
	                                     0.0f);

	// Trace downward from the target location to offset the target location,
	// preventing the lower half of the capsule from going through the floor when the ragdoll is laying on the ground.
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
	                        TargetRagdollLocation.Z - CapsuleComp->GetScaledCapsuleHalfHeight());

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
	                                                  ECC_Visibility, Params);

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += CapsuleComp->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}
	if (RagdollVar.GetValueOnGameThread())
	{
		DrawDebugSphere(World, NewRagdollLoc, 30, 1, FColor::Red);
		DrawDebugSphere(World, TargetRagdollLocation, 30, 1, FColor::Green);
	}
	GetCharacterOwner()->SetActorLocationAndRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation,
	                                                 TargetRagdollRotation);
}
#pragma endregion
