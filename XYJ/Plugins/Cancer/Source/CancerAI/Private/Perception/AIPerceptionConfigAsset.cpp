#include "Perception/AIPerceptionConfigAsset.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "TeamConfigDataAsset.h"
#include "Components/CancerTeamComponent.h"


UE_DISABLE_OPTIMIZATION

static bool ActorHasAllNameTags(const AActor* Actor, const TArray<FName>& Required)
{
	if (!IsValid(Actor)) return false;
	if (Required.Num() == 0) return true;
	const TArray<FName>& Owned = Actor->Tags;
	for (const FName& Tag : Required)
	{
		if (!Owned.Contains(Tag))
		{
			return false;
		}
	}
	return true;
}

static bool ActorHasAnyBlockedNameTag(const AActor* Actor, const TArray<FName>& Blocked)
{
	if (!IsValid(Actor)) return false;
	if (Blocked.Num() == 0) return false;
	const TArray<FName>& Owned = Actor->Tags;
	for (const FName& Tag : Blocked)
	{
		if (Owned.Contains(Tag))
		{
			return true;
		}
	}
	return false;
}

void UAIPerceptionConfigAsset::ApplyToController(AAIController* Controller) const
{
	if (!Controller)
	{
		return;
	}

	// Ensure controller has a perception component
	UAIPerceptionComponent* Perception = Controller->FindComponentByClass<UAIPerceptionComponent>();
	if (!Perception)
	{
		Perception = NewObject<UAIPerceptionComponent>(Controller, UAIPerceptionComponent::StaticClass(),
		                                               TEXT("PerceptionComponent"));
		if (Perception)
		{
			Perception->RegisterComponent();
		}
	}
	if (!Perception)
	{
		return;
	}

	for (auto SightConfig : SensesConfigs)
	{
		Perception->ConfigureSense(*SightConfig);
	}
	Perception->SetDominantSense(UAISense_Sight::StaticClass());
	Perception->RequestStimuliListenerUpdate();
}

bool UAIPerceptionConfigAsset::ShouldAcceptTarget(const AAIController* Controller, const AActor* Target) const
{

	if (!ActorHasAllNameTags(Target, RequiredActorTags))
	{
		return false;
	}
	if (ActorHasAnyBlockedNameTag(Target, BlockedActorTags))
	{
		return false;
	}
	const UCancerTeamComponent* SelfTeamComp = Controller->FindComponentByClass<UCancerTeamComponent>();
	if (!SelfTeamComp && Controller->GetPawn())
	{
		SelfTeamComp = Controller->GetPawn()->FindComponentByClass<UCancerTeamComponent>();
	}
	const UCancerTeamComponent* OtherTeamComp = Target->FindComponentByClass<UCancerTeamComponent>();
	if (SelfTeamComp && OtherTeamComp && SelfTeamComp->GetTeamsConfiguration())
	{
		const ETeamAttitude::Type Att = SelfTeamComp->GetAttitude(SelfTeamComp->TeamTag, OtherTeamComp->TeamTag);
		switch (Att)
		{
		case ETeamAttitude::Hostile:
			return bDetectEnemies;
		case ETeamAttitude::Neutral:
			return bDetectNeutrals;
		case ETeamAttitude::Friendly:
			return bDetectFriendlies;
		default:
			break;
		}
	}

    // If no team info available, default accept
    return false;
}

UE_ENABLE_OPTIMIZATION
