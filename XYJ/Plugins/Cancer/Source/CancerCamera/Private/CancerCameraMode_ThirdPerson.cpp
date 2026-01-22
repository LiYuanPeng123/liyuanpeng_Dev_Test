// Copyright Epic Games, Inc. All Rights Reserved.

#include "CancerCameraMode_ThirdPerson.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/CameraBlockingVolume.h"
#include "CancerCameraAssistInterface.h"
#include "CancerCameraPenetrationData.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "Math/RotationMatrix.h"
#include "Engine/GameViewportClient.h"


static TAutoConsoleVariable<bool> CameraVar(TEXT("Cancer.Camera"), false,TEXT(""));

#include UE_INLINE_GENERATED_CPP_BY_NAME(CancerCameraMode_ThirdPerson)

namespace CancerCameraMode_ThirdPerson_Statics
{
	static const FName NAME_IgnoreCameraCollision = TEXT("IgnoreCameraCollision");
}

DISABLE_FUNCTION_OPTIMIZATION
UCancerCameraMode_ThirdPerson::UCancerCameraMode_ThirdPerson()
{
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(+00.0f, +00.0f, 0.0f), 1.00f, 1.00f, 14.f, 0));
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(+00.0f, +16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(+00.0f, -16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(+00.0f, +32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(+00.0f, -32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(+20.0f, +00.0f, 0.0f), 1.00f, 1.00f, 00.f, 4));
	PenetrationAvoidanceFeelers.Add(
		FCancerPenetrationAvoidanceFeeler(FRotator(-20.0f, +00.0f, 0.0f), 0.50f, 0.50f, 00.f, 4));
}

bool UCancerCameraMode_ThirdPerson::GetLockOnTargetLoc(FVector& TargetPos) const
{
	if (!bEnableLockOn) return false;
	if (bEnableLockOnLoc)
	{
		TargetPos = LockOnTargetLoc;
		return true;
	}
	if (LockOnTarget.IsValid())
	{
		TargetPos = LockOnTarget->GetActorLocation();
		if (UMeshComponent* Mesh = LockOnTarget->FindComponentByTag<UMeshComponent>(LockingComponentName))
		{
			if (Mesh->DoesSocketExist(LockingSocketName))
			{
				TargetPos = Mesh->GetSocketLocation(LockingSocketName);
			}
		}
		return true;
	}
	return false;
}

void UCancerCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	//基础位置和旋转
	FRotator DesiredRot = GetPivotRotation();
	FVector PivotLoc = GetPivotLocation();
	//锁定位置
	FVector TargetPos;
	bool HasLockTarget = GetLockOnTargetLoc(TargetPos);
	if (HasLockTarget)
	{
		//锁定点偏移
		TargetPos += LockOnOffset;

		// 预测目标位置，减少受击或快速移动导致的旋转突变
		if (bLockOnUsePredictiveTarget && LockOnTarget.IsValid())
		{
			const FVector TargetVel = LockOnTarget->GetVelocity();
			TargetPos += TargetVel * LockOnPredictionTime;
		}

		FRotator TargetLookAtRot = (TargetPos - PivotLoc).Rotation();

		if (bUseScreenSpaceLockOffset)
		{
			// 将目标在屏幕上的偏移映射为角度偏移，避免因距离变化导致的 pitch 放大
			// 使用视口纵向 FOV 估算横向 FOV
			float AspectRatio = 16.f / 9.f;
			if (UGameViewportClient* GVC = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
			{
				FVector2D VPSize(1920.f, 1080.f);
				GVC->GetViewportSize(VPSize);
				if (VPSize.Y > KINDA_SMALL_NUMBER)
				{
					AspectRatio = VPSize.X / VPSize.Y;
				}
			}
			const float VerticalFOVRadHalf = FMath::DegreesToRadians(FieldOfView) * 0.5f;
			const float TanVert = FMath::Tan(VerticalFOVRadHalf);
			const float TanHorz = TanVert * AspectRatio;
			const float YawOffsetRad = FMath::Atan(LockOnScreenOffset.X * TanHorz);
			const float PitchOffsetRad = FMath::Atan(LockOnScreenOffset.Y * TanVert);
			TargetLookAtRot.Yaw += FMath::RadiansToDegrees(YawOffsetRad);
			// 屏幕空间 Y 正向为“向上”，为了让目标显示在屏幕上方，需要相机俯视（负 pitch）
			TargetLookAtRot.Pitch -= FMath::RadiansToDegrees(PitchOffsetRad);
		}
		else
		{
			FVector Up = FRotationMatrix(TargetLookAtRot).GetScaledAxis(EAxis::Z);
			FVector Right = FRotationMatrix(TargetLookAtRot).GetScaledAxis(EAxis::Y);
			// 世界空间偏移（原逻辑）：近距离会导致 pitch 放大
			FVector TargetSocketOffset = Up * LockOnOffset_Ver + Right * LockOnOffset_Hor + PivotLoc;
			TargetLookAtRot = (TargetPos - TargetSocketOffset).Rotation();
		}

		TargetLookAtRot.Roll = 0;

		if (CameraVar.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorld(), TargetPos, 10.f, 12, FColor::Green);
			DrawDebugLine(GetWorld(), PivotLoc, TargetPos, FColor::White);
		}

		// 角速度限制避免大幅摆动
		FRotator GoalRot = PreviousLockOnRot.IsNearlyZero() ? TargetLookAtRot : PreviousLockOnRot;
		if (bLockOnClampAngularSpeed && !PreviousLockOnRot.IsNearlyZero())
		{
			const float MaxYawDelta = LockOnMaxYawSpeed * DeltaTime;
			const float MaxPitchDelta = LockOnMaxPitchSpeed * DeltaTime;
			const float YawDelta = FMath::FindDeltaAngleDegrees(GoalRot.Yaw, TargetLookAtRot.Yaw);
			const float PitchDelta = FMath::FindDeltaAngleDegrees(GoalRot.Pitch, TargetLookAtRot.Pitch);
			GoalRot.Yaw += FMath::Clamp(YawDelta, -MaxYawDelta, MaxYawDelta);
			GoalRot.Pitch += FMath::Clamp(PitchDelta, -MaxPitchDelta, MaxPitchDelta);
		}
		else
		{
			GoalRot = TargetLookAtRot;
		}

		// Pitch 限制，避免上下过度摇摆
		GoalRot.Pitch = FMath::ClampAngle(GoalRot.Pitch, LockOnPitchMin, LockOnPitchMax);
		GoalRot.Roll = 0.f;

		// 平滑到目标旋转
		if (!PreviousLockOnRot.IsNearlyZero())
		{
			FQuat SmoothQuat = FMath::QInterpTo(FQuat(PreviousLockOnRot), FQuat(GoalRot), DeltaTime, LockOnInterpSpeed);
			SmoothQuat.Normalize();
			DesiredRot = SmoothQuat.Rotator();
		}
		else
		{
			DesiredRot = GoalRot;
		}
		PreviousLockOnRot = DesiredRot;
	}
	else
	{
		// 移动回正：在移动时，Yaw在一定范围内向角色前方插值
		if (bAutoRecenterWhileMoving)
		{
			if (const AActor* TargetActorLocal = GetTargetActor())
			{
				float Speed2D = 0.f;
				bool bIsMoving = false;

				if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetActorLocal))
				{
					Speed2D = TargetCharacter->GetVelocity().Size2D();
					bIsMoving = (Speed2D >= MoveRecenterMinSpeed2D) || (TargetCharacter->GetLastMovementInputVector().
						SizeSquared() > 0.f);
				}
				else if (const APawn* TargetPawn = Cast<APawn>(TargetActorLocal))
				{
					Speed2D = FVector(TargetPawn->GetVelocity().X, TargetPawn->GetVelocity().Y, 0.f).Size();
					bIsMoving = (Speed2D >= MoveRecenterMinSpeed2D) || (TargetPawn->GetLastMovementInputVector().
						SizeSquared() > 0.f);
				}

				if (bIsMoving)
				{
					const float ActorYaw = TargetActorLocal->GetActorRotation().Yaw;
					const float YawDelta = FMath::FindDeltaAngleDegrees(DesiredRot.Yaw, ActorYaw);

					if (FMath::Abs(YawDelta) <= MoveRecenterYawRangeMax && FMath::Abs(YawDelta) >=
						MoveRecenterYawRangeMin)
					{
						const FRotator CurrentYawOnly(0.f, DesiredRot.Yaw, 0.f);
						const FRotator TargetYawOnly(0.f, ActorYaw, 0.f);
						const FRotator NewYawOnly = FMath::RInterpTo(CurrentYawOnly, TargetYawOnly, DeltaTime,
						                                             MoveRecenterInterpSpeed);
						DesiredRot.Yaw = NewYawOnly.Yaw;
					}
				}
			}
		}

		if (bRotLag)
		{
			if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraRotationLagSpeed > 0.f)
			{
				const FRotator ArmRotStep = (DesiredRot - PreviousDesiredRot).GetNormalized() * (1.f / DeltaTime);
				FRotator LerpTarget = PreviousDesiredRot;
				float RemainingTime = DeltaTime;
				while (RemainingTime > UE_KINDA_SMALL_NUMBER)
				{
					const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
					LerpTarget += ArmRotStep * LerpAmount;
					RemainingTime -= LerpAmount;

					//DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(LerpTarget), LerpAmount, CameraRotationLagSpeed));
					FQuat TargetQuat = FQuat(DesiredRot);
					FQuat SmoothedQuat = FMath::QInterpTo(FQuat(PreviousDesiredRot), TargetQuat, LerpAmount,
					                                      CameraRotationLagSpeed);
					SmoothedQuat.Normalize();
					DesiredRot = SmoothedQuat.Rotator();
					DesiredRot.Roll = 0.f; // 强制清零
					PreviousDesiredRot = DesiredRot;
					PreviousLockOnRot = FRotator::ZeroRotator; // 重置
				}
			}
			else
			{
				DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(DesiredRot), DeltaTime,
				                                       CameraRotationLagSpeed));
			}
		}
	}

	PreviousDesiredRot = DesiredRot;

	// Get the spring arm 'origin', the target we want to look at
	FVector ArmOrigin = GetPivotLocation() + PreviousDesiredRot.RotateVector(TargetOffset);
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	FVector DesiredLoc = ArmOrigin;
	if (bDoLocationLag)
	{
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraLagSpeed > 0.f)
		{
			const FVector ArmMovementStep = (DesiredLoc - PreviousDesiredLoc) * (1.f / DeltaTime);
			FVector LerpTarget = PreviousDesiredLoc;

			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmMovementStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, LerpTarget, LerpAmount, CameraLagSpeed);
				PreviousDesiredLoc = DesiredLoc;
			}
		}
		else
		{
			DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
		}

		// Clamp distance if requested
		bool bClampedDist = false;
		if (CameraLagMaxDistance > 0.f)
		{
			const FVector FromOrigin = DesiredLoc - ArmOrigin;
			if (FromOrigin.SizeSquared() > FMath::Square(CameraLagMaxDistance))
			{
				DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
				bClampedDist = true;
			}
		}
		if (CameraVar.GetValueOnGameThread())
		{
			const FVector ToOrigin = ArmOrigin - DesiredLoc;
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, DesiredLoc + ToOrigin * 0.5f, 7.5f,
			                          bClampedDist ? FColor::Red : FColor::Green);
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc + ToOrigin * 0.5f, ArmOrigin, 7.5f,
			                          bClampedDist ? FColor::Red : FColor::Green);
		}
	}
	if (CameraVar.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), ArmOrigin, 5.f, 8, FColor::Green);
		DrawDebugSphere(GetWorld(), DesiredLoc, 5.f, 8, FColor::Yellow);
	}
	PreviousArmOrigin = ArmOrigin;
	PreviousDesiredLoc = DesiredLoc;

	// Now offset camera position back along our rotation
	DesiredLoc -= DesiredRot.Vector() * TargetArmLength;
	// Add socket offset in local space
	DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SocketOffset);

	if (CameraVar.GetValueOnGameThread())
	{
		const FVector ToOrigin = ArmOrigin - DesiredLoc;
		DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, PreviousArmOrigin + ToOrigin * 0.5f, 7.5f,
		                          FColor::Green);
		DrawDebugDirectionalArrow(GetWorld(), DesiredLoc + ToOrigin * 0.5f, PreviousArmOrigin, 7.5f,
		                          FColor::Green);
	}

	View.Location = DesiredLoc;
	View.Rotation = DesiredRot;
	View.ControlRotation = DesiredRot;
	View.FieldOfView = FieldOfView;
	UpdatePreventPenetration(DeltaTime);
	if (CameraVar.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), DesiredLoc, 5.f, 8, FColor::Blue);

		if (HasLockTarget)
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, TargetPos, 7.5f,
			                          FColor::White);
	}
}

void UCancerCameraMode_ThirdPerson::OnActivation()
{
	Super::OnActivation();
	if (PenetrationData)
	{
		PenetrationAvoidanceFeelers = PenetrationData->PenetrationAvoidanceFeelers;
		CollisionPushOutDistance = PenetrationData->CollisionPushOutDistance;
		PenetrationBlendInTime = PenetrationData->PenetrationBlendInTime;
		PenetrationBlendOutTime = PenetrationData->PenetrationBlendOutTime;
		ReportPenetrationPercent = PenetrationData->ReportPenetrationPercent;
	}
}


void UCancerCameraMode_ThirdPerson::DrawDebug(UCanvas* Canvas) const
{
	Super::DrawDebug(Canvas);

	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;


	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("      CancerCameraMode: %s (%f)"), *GetName(), BlendWeight));

	DisplayDebugManager.DrawString(FString::Printf(TEXT("      锁定点位置: %s"), *LockOnTargetLoc.ToString()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("      锁定点距离: %f"),
	                                               FVector::Distance(LockOnTargetLoc, GetPivotLocation())));


	for (int i = 0; i < DebugActorsHitDuringCameraPenetration.Num(); i++)
	{
		DisplayDebugManager.DrawString(
			FString::Printf(TEXT("HitActorDuringPenetration[%d]: %s")
			                , i
			                , *DebugActorsHitDuringCameraPenetration[i]->GetName()));
	}

#if ENABLE_DRAW_DEBUG
	LastDrawDebugTime = GetWorld()->GetTimeSeconds();
#endif
}

void UCancerCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)
{
	if (!bPreventPenetration)
	{
		return;
	}

	AActor* TargetActor = GetTargetActor();

	APawn* TargetPawn = Cast<APawn>(TargetActor);
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;

	ICancerCameraAssistInterface* TargetActorAssist = Cast<ICancerCameraAssistInterface>(TargetActor);

	AActor* OptionalPPTarget = TargetActorAssist
		                           ? TargetActorAssist->GetCameraPreventPenetrationTarget()
		                           : nullptr;
	AActor* PPActor = OptionalPPTarget ? OptionalPPTarget : TargetActor;

	const UPrimitiveComponent* PPActorRootComponent = Cast<UPrimitiveComponent>(PPActor->GetRootComponent());
	if (PPActorRootComponent)
	{
		// Attempt at picking SafeLocation automatically, so we reduce camera translation when aiming.
		// Our camera is our reticle, so we want to preserve our aim and keep that as steady and smooth as possible.
		// Pick closest point on capsule to our aim line.
		// 尝试自动选择安全位置，这样在瞄准时就能减少相机的移动。
		// 我们的相机就是准星，所以我们希望保持瞄准的稳定性，使其尽可能平稳流畅。
		// 选择胶囊体上距离瞄准线最近的点。
		FVector ClosestPointOnLineToCapsuleCenter;
		FVector SafeLocation = PreviousDesiredLoc; /*PPActor->GetActorLocation();*/
		FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);

		// Adjust Safe distance height to be same as aim line, but within capsule.
		// 将安全距离高度调整为与瞄准线相同，但需在胶囊体范围内。
		float const PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
		float const MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;
		SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight,
		                              SafeLocation.Z + MaxHalfHeight);

		float DistanceSqr;
		PPActorRootComponent->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr,
		                                                    SafeLocation);
		// Push back inside capsule to avoid initial penetration when doing line checks.
		// 在进行线路检查时，退回舱内以避免初始穿透。
		if (PenetrationAvoidanceFeelers.Num() > 0)
		{
			SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
		}

		// Then aim line to desired camera position
		// 然后将瞄准线调整至期望的摄像机位置
		bool const bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;
		PreventCameraPenetration(*PPActor, SafeLocation, View.Location, DeltaTime, AimLineToDesiredPosBlockedPct,
		                         bSingleRayPenetrationCheck);

		AActor* Actors[] = {TargetController, TargetActor, PPActor};
		if (AimLineToDesiredPosBlockedPct < ReportPenetrationPercent)
		{
			for (auto Actor : Actors)
			{
				if (Actor && Actor->Implements<UCancerCameraAssistInterface>())
				{
					ICancerCameraAssistInterface::Execute_OnCameraPenetratingTarget(Actor);
				}
			}
		}
	}
}

void UCancerCameraMode_ThirdPerson::PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc,
                                                             FVector& CameraLoc, float const& DeltaTime,
                                                             float& DistBlockedPct, bool bSingleRayOnly)
{
#if ENABLE_DRAW_DEBUG
	DebugActorsHitDuringCameraPenetration.Reset();
#endif

	float HardBlockedPct = DistBlockedPct;
	float SoftBlockedPct = DistBlockedPct;

	FVector BaseRay = CameraLoc - SafeLoc;
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
	FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;

	BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);

	float DistBlockedPctThisFrame = 1.f;

	int32 const NumRaysToShoot = bSingleRayOnly
		                             ? FMath::Min(1, PenetrationAvoidanceFeelers.Num())
		                             : PenetrationAvoidanceFeelers.Num();
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr/*PlayerCamera*/);

	SphereParams.AddIgnoredActor(&ViewTarget);

	//如果（忽略摄像机穿透的演员）
	//TODO ICancerCameraTarget.GetIgnoredActorsForCameraPentration();
	//if (IgnoreActorForCameraPenetration)
	//{
	//	SphereParams.AddIgnoredActor(IgnoreActorForCameraPenetration);
	//}

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);
	UWorld* World = GetWorld();

	for (int32 RayIdx = 0; RayIdx < NumRaysToShoot; ++RayIdx)
	{
		FCancerPenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIdx];
		if (Feeler.FramesUntilNextTrace <= 0)
		{
			// calc ray target
			FVector RayTarget;
			{
				FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
				RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
				RayTarget = SafeLoc + RotatedRay;
			}

			// cast for world and pawn hits separately.  this is so we can safely ignore the 
			// camera's target pawn
			// 分别为世界和棋子碰撞进行投射。这样我们就可以安全地忽略摄像机的目标棋子
			SphereShape.Sphere.Radius = Feeler.Extent;
			ECollisionChannel TraceChannel = ECC_Camera; //(Feeler.PawnWeight > 0.f) ? ECC_Pawn : ECC_Camera;

			// do multi-line check to make sure the hits we throw out aren't
			// masking real hits behind (these are important rays).

			// MT-> passing camera as actor so that camerablockingvolumes know when it's the camera doing traces
			// 进行多行检查，以确保我们排除的命中不会掩盖后面的真正命中（这些射线很重要）。
			// MT-> 将摄像机作为角色传递，以便摄像机阻挡体知道何时是摄像机在进行追踪
			FHitResult Hit;
			const bool bHit = World->SweepSingleByChannel(Hit, SafeLoc, RayTarget, FQuat::Identity, TraceChannel,
			                                              SphereShape, SphereParams);

			if (CameraVar.GetValueOnGameThread())
			{
				DrawDebugSphere(World, SafeLoc, SphereShape.Sphere.Radius, 8, FColor::Red);
				DrawDebugSphere(World, bHit ? Hit.Location : RayTarget, SphereShape.Sphere.Radius, 8, FColor::Red);
				DrawDebugLine(World, SafeLoc, bHit ? Hit.Location : RayTarget, FColor::Red);
			}

			Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

			const AActor* HitActor = Hit.GetActor();

			if (bHit && HitActor)
			{
				bool bIgnoreHit = false;

				if (HitActor->ActorHasTag(CancerCameraMode_ThirdPerson_Statics::NAME_IgnoreCameraCollision))
				{
					bIgnoreHit = true;
					SphereParams.AddIgnoredActor(HitActor);
				}

				// Ignore CameraBlockingVolume hits that occur in front of the ViewTarget.
				if (!bIgnoreHit && HitActor->IsA<ACameraBlockingVolume>())
				{
					const FVector ViewTargetForwardXY = ViewTarget.GetActorForwardVector().GetSafeNormal2D();
					const FVector ViewTargetLocation = ViewTarget.GetActorLocation();
					const FVector HitOffset = Hit.Location - ViewTargetLocation;
					const FVector HitDirectionXY = HitOffset.GetSafeNormal2D();
					const float DotHitDirection = FVector::DotProduct(ViewTargetForwardXY, HitDirectionXY);
					if (DotHitDirection > 0.0f)
					{
						bIgnoreHit = true;
						// Ignore this CameraBlockingVolume on the remaining sweeps.
						SphereParams.AddIgnoredActor(HitActor);
					}
					else
					{
#if ENABLE_DRAW_DEBUG
						DebugActorsHitDuringCameraPenetration.AddUnique(TObjectPtr<const AActor>(HitActor));
#endif
					}
				}

				if (!bIgnoreHit)
				{
					float const Weight = Cast<APawn>(Hit.GetActor()) ? Feeler.PawnWeight : Feeler.WorldWeight;
					float NewBlockPct = Hit.Time;
					NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

					// Recompute blocked pct taking into account pushout distance.
					// 考虑推出距离后重新计算阻塞百分比。
					NewBlockPct = ((Hit.Location - SafeLoc).Size() - CollisionPushOutDistance) / (RayTarget - SafeLoc).
						Size();
					DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

					// This feeler got a hit, so do another trace next frame
					// 这次探测器成功触发了，所以下一帧要再进行一次探测。
					Feeler.FramesUntilNextTrace = 0;

#if ENABLE_DRAW_DEBUG
					DebugActorsHitDuringCameraPenetration.AddUnique(TObjectPtr<const AActor>(HitActor));
#endif
				}
			}

			if (RayIdx == 0)
			{
				// don't interpolate toward this one, snap to it
				// assumes ray 0 is the center/main ray
				// 不要向这一侧倾斜，而是与之垂直对齐
				// 假定第 0 条射线是中心/主射线
				HardBlockedPct = DistBlockedPctThisFrame;
			}
			else
			{
				SoftBlockedPct = DistBlockedPctThisFrame;
			}
		}
		else
		{
			--Feeler.FramesUntilNextTrace;
		}
	}

	if (bResetInterpolation)
	{
		DistBlockedPct = DistBlockedPctThisFrame;
	}
	else if (DistBlockedPct < DistBlockedPctThisFrame)
	{
		// interpolate smoothly out
		// 平滑地渐出
		if (PenetrationBlendOutTime > DeltaTime)
		{
			DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame -
				DistBlockedPct);
		}
		else
		{
			DistBlockedPct = DistBlockedPctThisFrame;
		}
	}
	else
	{
		if (DistBlockedPct > HardBlockedPct)
		{
			DistBlockedPct = HardBlockedPct;
		}
		else if (DistBlockedPct > SoftBlockedPct)
		{
			// interpolate smoothly in
			// 以平滑的方式进行插值
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct -
					SoftBlockedPct);
			}
			else
			{
				DistBlockedPct = SoftBlockedPct;
			}
		}
	}

	DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.f);
	if (DistBlockedPct < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		CameraLoc = SafeLoc + (CameraLoc - SafeLoc) * DistBlockedPct;
	}
}
UE_ENABLE_OPTIMIZATION