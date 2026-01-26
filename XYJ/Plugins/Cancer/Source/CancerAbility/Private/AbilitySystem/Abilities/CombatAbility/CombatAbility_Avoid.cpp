
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
