
#include "CancerAbilityCheats.h"

#include "Components/CancerCombatAttributeComponent.h"

UCancerAbilityCheats::UCancerAbilityCheats()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void UCancerAbilityCheats::DamageSelfDestruct()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER

	if (auto CombatAttributeComponent = GetCombatAttributeComponent())
	{
		
	}
#endif
}

UCancerCombatAttributeComponent* UCancerAbilityCheats::GetCombatAttributeComponent()
{
	if (APlayerController* PC = GetPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			return Pawn->FindComponentByClass<UCancerCombatAttributeComponent>();
		}
	}

	return nullptr;
}
