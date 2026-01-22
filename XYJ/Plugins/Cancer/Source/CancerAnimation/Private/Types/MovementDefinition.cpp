

#include "Types/MovementDefinition.h"

#include "Components/CancerMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

float UCancerMovementNode::GetMaxWalkSpeed() const
{
	return 0;
}

float UCancerMovementNode::GetMaxWalkSpeedCrouched() const
{
	return 0;
}

void UCancerMovementNode::SetMaxWalkSpeed(float Speed) 
{
}

void UCancerMovementNode::SetMaxWalkSpeedCrouched(float Speed) 
{
}

void UCancerMovementNode1::PreUpdateMovementParameter(UCancerMovementComponent* Movement)
{
	Super::PreUpdateMovementParameter(Movement);
	if (!Movement) return;
	Movement->MaxWalkSpeedCrouched = MovementParameter.MaxWalkSpeedCrouched;
	Movement->MaxAcceleration = MovementParameter.MaxAcceleration;
	Movement->BrakingDecelerationWalking = MovementParameter.BrakingDecelerationWalking;
	Movement->GroundFriction = MovementParameter.GroundFriction;
	Movement->BrakingFrictionFactor = MovementParameter.BrakingFrictionFactor;
	Movement->bUseSeparateBrakingFriction = MovementParameter.bUseSeparateBrakingFriction;
	Movement->BrakingFriction = MovementParameter.BrakingFriction;
	Movement->MaxWalkSpeed = MovementParameter.MaxWalkSpeed;
	if (!Movement->bOrientToMovement)
	{
		//if (UKismetMathLibrary::LessLess_VectorRotator(Movement->GetLastUpdateRotation(),Movement->LastVelocityRotation))
		//Movement->MaxWalkSpeed = MovementParameter.MaxWalkSpeed;
	}
}

void UCancerMovementNode1::PostUpdateMovementParameter(UCancerMovementComponent* Movement)
{
	Super::PostUpdateMovementParameter(Movement);
}

void UCancerMovementNode1::UpdateMovementRotation(UCancerMovementComponent* Movement, bool bUseCurveRotation)
{
	Super::UpdateMovementRotation(Movement, bUseCurveRotation);
	if (!Movement) return;
	if (!bUseCurveRotation)
	{
		Movement->RotationRate = MovementParameter.RotationRate;
	}
}

float UCancerMovementNode1::GetMaxWalkSpeed() const
{
	return MovementParameter.MaxWalkSpeed;
}

float UCancerMovementNode1::GetMaxWalkSpeedCrouched() const
{
	return MovementParameter.MaxWalkSpeedCrouched;
}

void UCancerMovementNode1::SetMaxWalkSpeed(float Speed) 
{
	MovementParameter.MaxWalkSpeed = Speed;
}

void UCancerMovementNode1::SetMaxWalkSpeedCrouched(float Speed) 
{
	MovementParameter.MaxWalkSpeedCrouched = Speed;
}
