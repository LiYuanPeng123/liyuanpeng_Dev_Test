#include "Providers/Animation/AnimationProvider_DirectionalEvade.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/CancerCoreSystemInterface.h"
#include "Components/CancerHeroComponent.h"

UAnimationProvider_DirectionalEvade::UAnimationProvider_DirectionalEvade()
{
	Offset = 0.f;
}

bool UAnimationProvider_DirectionalEvade::CalculateAnimationAngle_Implementation(
	const UCancerGameplayAbility* CombatAbility, float& OutAngle) const
{
	const FGameplayAbilityActorInfo* MyActorInfo = CombatAbility->GetCurrentActorInfo();
	if (MyActorInfo == nullptr) return false;

	const UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(MyActorInfo->MovementComponent);
	if (Movement == nullptr) return false;

	FVector Acceleration = Movement->GetCurrentAcceleration();
	Acceleration.Normalize();

	if (Acceleration.IsZero())
	{
		OutAngle = 0.f;
		return false;
	}

	const AActor* AvatarActor = CombatAbility->GetAvatarActorFromActorInfo();

	// 优先使用：锁定目标方向（角色->目标 的二维朝向）
	float ReferenceYaw = 0.f;
	bool bHasReferenceYaw = false;
	if (const AActor* LockedTarget = UCancerHeroComponent::FindHeroComponent(AvatarActor)->GetCombatTarget())
	{
		FVector ToTarget = LockedTarget->GetActorLocation() - AvatarActor->GetActorLocation();
		ToTarget.Z = 0.f;
		if (!ToTarget.IsNearlyZero())
		{
			ReferenceYaw = ToTarget.ToOrientationRotator().Yaw;
			bHasReferenceYaw = true;
		}
	}

	// 次选：战斗前向参考组件的朝向
	if (!bHasReferenceYaw)
	{
		const USceneComponent* ForwardReference = ICancerCoreSystemInterface::Execute_GetCombatForwardReference(
			const_cast<AActor*>(AvatarActor));
		if (ForwardReference)
		{
			ReferenceYaw = ForwardReference->GetComponentRotation().Yaw;
			bHasReferenceYaw = true;
		}
	}

	//使用角色自身朝向
	if (!bHasReferenceYaw)
	{
		ReferenceYaw = Movement->GetOwner()->GetActorRotation().Yaw;
	}

	const float OwnerYaw = Movement->GetOwner()->GetActorRotation().Yaw;
	const float Angle = ReferenceYaw - OwnerYaw + Offset;

	const FVector YawAxis = FVector(0, 0, 1);
	const FVector RotatedInput = Acceleration.RotateAngleAxis(Angle, YawAxis);
	const FRotator InputOrientation = RotatedInput.ToOrientationRotator();

	const float EvadeDirection = InputOrientation.Yaw;

	OutAngle = EvadeDirection;

	return true;
}
