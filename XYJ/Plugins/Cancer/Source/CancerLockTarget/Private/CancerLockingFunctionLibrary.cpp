

#include "CancerLockingFunctionLibrary.h"

#include "CancerLockTargetSetting.h"
#include "Components/CancerLockingPointComponent.h"
#include "Components/CancerLockingTargetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

class UCancerLockingTargetComponent* UCancerLockingFunctionLibrary::K2_GetLockingTargetComponent(AActor* Actor,
                                                                                                 bool& bIsValid, TSubclassOf<UCancerLockingTargetComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetLockingTargetComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerLockingTargetComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerLockingPointComponent* UCancerLockingFunctionLibrary::K2_GetLockTarget_PointComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerLockingPointComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetLockTarget_PointComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerLockingPointComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class AActor* UCancerLockingFunctionLibrary::K2_GetLockingTarget_Actor(AActor* Actor, bool& bIsValid,
	TSubclassOf<AActor> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetLockingTarget_Actor(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerLockingPointComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

class UCancerLockingPointComponent* UCancerLockingFunctionLibrary::K2_GetDefaultPointComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerLockingPointComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetDefaultPointComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerLockingPointComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

void UCancerLockingFunctionLibrary::SetLockingTarget_PointComponent(AActor* Actor,
	UCancerLockingPointComponent* LockingTarget)
{
	if (UCancerLockingTargetComponent* LockPointComponent = GetLockingTargetComponent(Actor))
	{
		LockPointComponent->SetLockingPointComponent(LockingTarget);
	}
}

void UCancerLockingFunctionLibrary::SetLockingTarget_Actor(AActor* Actor, AActor* LockingTarget)
{
	UCancerLockingPointComponent* LockingPointComponent = GetDefaultPointComponent(LockingTarget);
	SetLockingTarget_PointComponent(Actor, LockingPointComponent);
}

bool UCancerLockingFunctionLibrary::TraceBlockBetweenCollision(AActor* StartActor, AActor* EndActor,
	FVector2D DefaultStartSize, FVector2D DefaultEndSize, float Precision)
{
	if (!StartActor || !EndActor)
	{
		return false;
	}
	if (UCapsuleComponent* StartCapsule = StartActor->GetComponentByClass<UCapsuleComponent>())
	{
		DefaultStartSize.X = StartCapsule->GetScaledCapsuleRadius();
		DefaultStartSize.Y = StartCapsule->GetScaledCapsuleHalfHeight();
	}
	if (UCapsuleComponent* EndCapsule = EndActor->GetComponentByClass<UCapsuleComponent>())
	{
		DefaultEndSize.X = EndCapsule->GetScaledCapsuleRadius();
		DefaultEndSize.Y = EndCapsule->GetScaledCapsuleHalfHeight();
	}
	TArray<FVector> StartTraceVectors;
	TArray<FVector> EndTraceVectors;
	
	FVector StartLocationBase = StartActor->GetActorLocation();
	FVector EndLocationBase = EndActor->GetActorLocation();
	FVector StartUpVector = StartActor->GetActorUpVector();
	FVector EndUpVector = EndActor->GetActorUpVector();
	FVector StartRightVector = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::FindLookAtRotation(StartLocationBase, EndLocationBase));
	// FVector StartRightVector = StartActor->GetActorRightVector();
	// FVector EndRightVector = EndActor->GetActorRightVector();
	
	StartTraceVectors.Emplace(StartLocationBase);
	StartTraceVectors.Emplace(StartLocationBase + StartUpVector * (DefaultStartSize.Y - 10));
	StartTraceVectors.Emplace(StartLocationBase - StartUpVector * (DefaultStartSize.Y - 10));
	StartTraceVectors.Emplace(StartLocationBase + StartRightVector * (DefaultStartSize.X - 10));
	StartTraceVectors.Emplace(StartLocationBase - StartRightVector * (DefaultStartSize.X - 10));
	
	EndTraceVectors.Emplace(EndLocationBase);
	EndTraceVectors.Emplace(EndLocationBase + EndUpVector * (DefaultEndSize.Y - 10));
	EndTraceVectors.Emplace(EndLocationBase - EndUpVector * (DefaultEndSize.Y - 10));
	EndTraceVectors.Emplace(EndLocationBase + StartRightVector * (DefaultEndSize.X - 10));
	EndTraceVectors.Emplace(EndLocationBase - StartRightVector * (DefaultEndSize.X - 10));
	
	// TEnumAsByte<ETraceTypeQuery> TraceType = GetDefault<UPanGuLockingSetting>()->DefaultLockingBlockTraceType;
	// TArray<TEnumAsByte<EObjectTypeQuery>> TraceObj = GetDefault<UPanGuLockingSetting>()->DefaultActionObstacleObjectTypes;
	TEnumAsByte<ETraceTypeQuery> TraceType = UCancerLockTargetSetting::Get()->DefaultLockingBlockTraceType;
	
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if (true)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}
	// bool bIsBlock = true;
	for (int i = 0; i < StartTraceVectors.Num(); i++)
	{
		if (Precision > 0.f)
		{
			;
			if (!UKismetSystemLibrary::SphereTraceSingle(StartActor, StartTraceVectors[i], EndTraceVectors[i],Precision, TraceType,
				false, ActorsToIgnore, DebugType, OutHit, true))
			{
				return false;
			}
		}
		else
		{
			if (!UKismetSystemLibrary::LineTraceSingle(StartActor, StartTraceVectors[i], EndTraceVectors[i], TraceType, false,
				ActorsToIgnore, DebugType, OutHit, true))
			{
				return false;
			}
		}
	}
	// return bIsBlock;
	return true;
}

bool UCancerLockingFunctionLibrary::TraceBlockByDirectionDistance(AActor* StartActor, FRotator TraceDirection,
	float TraceDistance, FVector TraceSize)
{
	FVector StartLoction = FVector(StartActor->GetActorLocation().X, StartActor->GetActorLocation().Y, StartActor->GetActorLocation().Z + TraceSize.Z);
	FVector EndLocation = StartLoction + UKismetMathLibrary::GreaterGreater_VectorRotator(StartActor->GetActorForwardVector(), TraceDirection).GetSafeNormal() * TraceDistance;
	FRotator Direction = UKismetMathLibrary::FindLookAtRotation(StartLoction, EndLocation);
	
	FVector UpVector = UKismetMathLibrary::GetUpVector(Direction);
	FVector RightVector = UKismetMathLibrary::GetRightVector(Direction);
	
	TArray<FVector> StartTraceVectors;
	TArray<FVector> EndTraceVectors;
	StartTraceVectors.Emplace(StartLoction);
	StartTraceVectors.Emplace(StartLoction + UpVector * (TraceSize.Y));
	StartTraceVectors.Emplace(StartLoction - UpVector * (TraceSize.Y));
	StartTraceVectors.Emplace(StartLoction + RightVector * (TraceSize.X));
	StartTraceVectors.Emplace(StartLoction - RightVector * (TraceSize.X));
	
	EndTraceVectors.Emplace(EndLocation);
	EndTraceVectors.Emplace(EndLocation + UpVector * (TraceSize.Y));
	EndTraceVectors.Emplace(EndLocation - UpVector * (TraceSize.Y));
	EndTraceVectors.Emplace(EndLocation + RightVector * (TraceSize.X));
	EndTraceVectors.Emplace(EndLocation - RightVector * (TraceSize.X));
	
	// TArray<TEnumAsByte<EObjectTypeQuery>> TraceObj = GetDefault<UPanGuLockingSetting>()->DefaultActionObstacleObjectTypes;
	TEnumAsByte<ETraceTypeQuery> TraceType = UCancerLockTargetSetting::Get()->DefaultLockingBlockTraceType;
	
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if (true)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}
	// bool bIsBlock = true;
	for (int i = 0; i < StartTraceVectors.Num(); i++)
	{
		// if (Precision > 0.f)
		// {
		// 	if (!UKismetSystemLibrary::SphereTraceSingleForObjects(StartActor, StartTraceVectors[i], EndTraceVectors[i],Precision, TraceObj,
		// 		false, ActorsToIgnore, DebugType, OutHit, true))
		// 	{
		// 		return false;
		// 	}
		// }
		// else
		// {
		if (!UKismetSystemLibrary::LineTraceSingle(StartActor, StartTraceVectors[i], EndTraceVectors[i], TraceType, false,
				ActorsToIgnore, DebugType, OutHit, true))
		{
			// bIsBlock = false;
			return false;
		}
		// }
	}

	// return bIsBlock;
	return true;
}

bool UCancerLockingFunctionLibrary::SectorTargetCheck(AActor* StartActor, AActor* TargetActor, float OffsetAngle,
	float CheckAngle, float CheckRadius, float CheckHeight)
{
	bool bIsCheck = false;
	FVector StartLocation = StartActor->GetActorLocation(); 
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector StartForward = StartActor->GetActorForwardVector();

	float RelativeDot = FVector::DotProduct(UKismetMathLibrary::Normal(FVector(TargetLocation.X, TargetLocation.Y, StartLocation.Z) - StartLocation),
		UKismetMathLibrary::Quat_RotateVector(FRotator(0.f, OffsetAngle, 0.f).Quaternion(), StartForward));
	
	float RelativeAngle = UKismetMathLibrary::DegAcos(RelativeDot);
	if (FVector::CrossProduct(StartForward, TargetLocation).Z > 0)
	{
		RelativeAngle *= -1;
	}

	if (TargetLocation.Z - StartLocation.Z <= CheckHeight && RelativeAngle >= - FMath::Abs(CheckAngle/2) && RelativeAngle <= FMath::Abs(CheckAngle/2))
	{
		if (CheckHeight > 0)
		{
			if (TargetLocation.Z - StartLocation.Z <= CheckHeight)
			{
				if (CheckRadius > 0)
				{
					if (FVector::Dist(FVector(TargetLocation.X, TargetLocation.Y, StartLocation.Z), StartLocation) <= CheckRadius)
					{
						bIsCheck = true;
					}
				}
				else
				{
					bIsCheck = true;
				}
			}
		}
		else
		{
			if (CheckRadius > 0)
			{
				if (FVector::Dist(FVector(TargetLocation.X, TargetLocation.Y, StartLocation.Z), StartLocation) <= CheckRadius)
				{
					bIsCheck = true;
				}
			}
			else
			{
				bIsCheck = true;
			}
		}
	}
	return bIsCheck;
}

bool UCancerLockingFunctionLibrary::IsCanLock(AActor* OwnerActor, AActor* TargetActor)
{
	auto LockComponent = GetLockingTargetComponent(OwnerActor);
	auto LockPointComponent = GetDefaultPointComponent(TargetActor);
	if (!LockComponent|| !LockPointComponent)
		return false;
	return LockComponent->IsCanLock(LockPointComponent);
}
