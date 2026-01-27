#include "AbilitySystem/Abilities/CombatAbility/CombatAbility_Avoid.h"
#include "Components/CancerMotionWarpingComponent.h"

void UCombatAbility_Avoid::OnAnimationReady_Implementation()
{
	Super::OnAnimationReady_Implementation();

	if (auto WarpComponent = GetMotionWarpingComponentFromActorInfo())
	{
		//添加Warp
		WarpComponent->RemoveWarpTarget(AvoidWarpName);
		WarpComponent->AddOrUpdateWarpTargetFromLocationAndRotation(AvoidWarpName, AvoidLocation, AvoidRotation);
	}
}

void UCombatAbility_Avoid::SetAvoidLocRot_Implementation(AActor* Attacker, AActor* Victim, FVector& Loc, FRotator& Rot)
{
	FVector AttackerLoc = Attacker->GetActorLocation();
	FVector VictimLoc = Victim->GetActorLocation();
	Rot = UKismetMathLibrary::FindLookAtRotation(VictimLoc, AttackerLoc);
	Rot.Pitch = Victim->GetActorRotation().Pitch;
	Rot.Roll = Victim->GetActorRotation().Roll;
	// 计算之间的平面距离和 Z 轴距离
	double Dis2D = UKismetMathLibrary::Vector_Distance2D(AttackerLoc, VictimLoc);
	double Z = VictimLoc.Z - AttackerLoc.Z;
	double DisZ = FMath::Abs(Z);

	// 距离比较近 不做水平位移 只做Z轴对齐
	if (Dis2D <= LocDis)
	{
		Loc = VictimLoc;
		Loc.Z = AttackerLoc.Z;
	}
	else
	{
		FVector Dir = (VictimLoc - AttackerLoc).GetSafeNormal2D();

		FVector RightDir = FVector(-Dir.Y, Dir.X, 0.0f);

		Loc = AttackerLoc + (Dir * TargetLoc.X) + (RightDir * TargetLoc.Y);
		Loc.Z = AttackerLoc.Z + TargetLoc.Z;
	}
}
