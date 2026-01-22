#include "Components/CancerMotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<bool> WarpAttackTargetVar(TEXT("Cancer.WarpAttackTarget"), false,TEXT(""));

void UCancerMotionWarpingComponent::UpdateLocationAndRotation_Actor(const AActor* Target, float MaxDis, FVector Offset,
                                                                    FName WarpName, bool bCancelWarpIfCloser)
{
	if (!IsValid(Target) || !IsValid(GetOwner()))
	{
		return;
	}
	const FVector TargetLocation = Target->GetActorLocation();
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	FVector Dir = OwnerLocation - TargetLocation;
	FVector Dir_Target = TargetLocation - OwnerLocation;

	Dir.Z = 0.f;
	Dir_Target.Z = 0.f;
	
	//水平高度采用自身的高度匹配
	FVector TargetPivot = FVector(TargetLocation.X,TargetLocation.Y,OwnerLocation.Z);

	Dir_Target.Normalize();
	if (FVector::Distance(TargetLocation, OwnerLocation) > MaxDis)
	{
		TargetPivot = OwnerLocation + MaxDis * Dir_Target;
	}
	FRotator Rotation = Dir.Rotation();

	//偏移：旋转为局部坐标系，把 Offset 转到世界并叠加到 Target 位置
	const FVector WorldOffset = Rotation.RotateVector(Offset);
	FVector WarpLocation = TargetPivot + WorldOffset;

	FRotator Rotation_Target = Dir_Target.Rotation();

	if (bCancelWarpIfCloser)
	{
		const float CurrentToTarget2D = FVector::Dist2D(OwnerLocation, TargetLocation);
		const float WarpToTarget2D = FVector::Dist2D(WarpLocation, TargetLocation);
		if (CurrentToTarget2D < WarpToTarget2D)
		{
			WarpLocation.X = OwnerLocation.X;
			WarpLocation.Y = OwnerLocation.Y;
			WarpLocation.Z = OwnerLocation.Z;
			AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, OwnerLocation, Rotation_Target);
		}
		else
		{
			AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, WarpLocation, Rotation_Target);
		}
	}
	if (WarpAttackTargetVar.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), WarpLocation, 20.f, 12, FColor::Blue, false, 2.f);
		const FVector Start = GetOwner()->GetActorLocation();
		const FVector End = WarpLocation;
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 15.f, FColor::Yellow,
		                          false, 2.f, 0, 2.0f);
		FVector WarpForward = UKismetMathLibrary::GetForwardVector(Rotation_Target);

		DrawDebugDirectionalArrow(GetWorld(), End, End + WarpForward * 30, 15.f, FColor::Red,
		                          false, 2.f, 0, 2.0f);
	}
}

void UCancerMotionWarpingComponent::UpdateLocationAndRotation_Character(const AActor* Target, float MaxDis,
                                                                        FVector Offset, FName WarpName,
                                                                        bool bCancelWarpIfCloser)
{
	auto TargetCharacter = Cast<ACharacter>(Target);
	if (!IsValid(TargetCharacter))
	{
		UpdateLocationAndRotation_Actor(Target, MaxDis, Offset, WarpName, bCancelWarpIfCloser);
		return;
	}
	const FVector TargetLocation = TargetCharacter->GetActorLocation();
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const float CapsuleRadius = TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();

	FVector Dir = OwnerLocation - TargetLocation;
	FVector Dir_Target = TargetLocation - OwnerLocation;

	FVector TargetPivot = TargetLocation;
	Dir_Target.Z = 0.f;
	Dir_Target.Normalize();
	if (FVector::Distance(TargetLocation, OwnerLocation) > MaxDis)
	{
		TargetPivot = OwnerLocation + MaxDis * Dir_Target;
	}
	
	Dir.Z = 0.f;
	FRotator Rotation = Dir.Rotation();

	// 偏移：旋转为局部坐标系，把 Offset 转到世界并叠加到 Target 位置 减去胶囊体半径的距离
	const FVector WorldOffset = Rotation.RotateVector(Offset);
	FVector WarpLocation = TargetPivot + WorldOffset - CapsuleRadius * Dir_Target;

	FRotator Rotation_Target = Dir_Target.Rotation();

	// 写入 Motion Warping 目标
	// 若启用取消逻辑：当前角色到目标距离 < 计算后的 Warp 位置到目标距离
	if (bCancelWarpIfCloser)
	{
		const float CurrentToTarget2D = FVector::Dist2D(OwnerLocation, TargetLocation);
		const float WarpToTarget2D = FVector::Dist2D(WarpLocation, TargetLocation);
		if (CurrentToTarget2D < WarpToTarget2D)
		{
			WarpLocation.X = OwnerLocation.X;
			WarpLocation.Y = OwnerLocation.Y;
			WarpLocation.Z = OwnerLocation.Z;
			AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, OwnerLocation, Rotation_Target);
		}
		else
		{
			AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, WarpLocation, Rotation_Target);
		}
	}


	if (WarpAttackTargetVar.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), WarpLocation, 20.f, 12, FColor::Blue, false, 2.f);
		const FVector Start = GetOwner()->GetActorLocation();
		const FVector End = WarpLocation;
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 15.f, FColor::Yellow,
		                          false, 2.f, 0, 2.0f);

		FVector WarpForward = UKismetMathLibrary::GetForwardVector(Rotation_Target);

		DrawDebugDirectionalArrow(GetWorld(), End, End + WarpForward * 30, 15.f, FColor::Red,
		                          false, 2.f, 0, 2.0f);
	}
}

void UCancerMotionWarpingComponent::UpdateLocationAndRotation_Self(FVector Offset, FName WarpName)
{
	auto TargetCharacter = Cast<ACharacter>(GetOwner());
	if (!IsValid(TargetCharacter) || !IsValid(GetOwner()))
	{
		return;
	}
	const FVector OwnerLocation = TargetCharacter->GetActorLocation();
	const FRotator YawRotation(0.f, TargetCharacter->GetActorRotation().Yaw, 0.f);
	const FVector TargetForward = UKismetMathLibrary::GetForwardVector(YawRotation);
	const FVector WorldOffset = YawRotation.RotateVector(Offset);
	FVector WarpLocation = OwnerLocation + TargetForward + WorldOffset; // 单位步长（用于无显式距离时）

	FRotator Rotation_Target = TargetForward.Rotation();

	AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, WarpLocation, Rotation_Target);

	if (WarpAttackTargetVar.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), WarpLocation, 20.f, 12, FColor::Blue, false, 2.f);
		const FVector Start = OwnerLocation;
		const FVector End = WarpLocation;
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 15.f, FColor::Yellow, false, 2.f, 0, 2.0f);
	}
}

void UCancerMotionWarpingComponent::UpdateLocationAndRotation_SelfDis(float Distance, FVector Offset, FName WarpName)
{
	auto TargetCharacter = Cast<ACharacter>(GetOwner());
	if (!IsValid(TargetCharacter) || !IsValid(GetOwner()))
	{
		return;
	}
	const FVector OwnerLocation = TargetCharacter->GetActorLocation();
	const FRotator YawRotation(0.f, TargetCharacter->GetActorRotation().Yaw, 0.f);
	const FVector TargetForward = UKismetMathLibrary::GetForwardVector(YawRotation);
	const FVector WorldOffset = YawRotation.RotateVector(Offset);
	FVector WarpLocation = OwnerLocation + TargetForward * FMath::Max(Distance, 0.f) + WorldOffset;

	FRotator Rotation_Target = TargetForward.Rotation();

	AddOrUpdateWarpTargetFromLocationAndRotation(WarpName, WarpLocation, Rotation_Target);

	if (WarpAttackTargetVar.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), WarpLocation, 20.f, 12, FColor::Blue, false, 2.f);
		const FVector Start = OwnerLocation;
		const FVector End = WarpLocation;
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 15.f, FColor::Yellow, false, 2.f, 0, 2.0f);
	}
}

void UCancerMotionWarpingComponent::SetPendingScaleDistance(float Distance)
{
	PendingScaleDistance = FMath::Max(Distance, 0.f);
}

float UCancerMotionWarpingComponent::ConsumePendingScaleDistance()
{
	const float Distance = PendingScaleDistance;
	PendingScaleDistance = -1.f;
	return Distance;
}
