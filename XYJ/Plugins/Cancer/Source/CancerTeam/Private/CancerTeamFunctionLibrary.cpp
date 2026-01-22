#include "CancerTeamFunctionLibrary.h"

#include "TeamConfigDataAsset.h"
#include "Components/CancerTeamComponent.h"

UCancerTeamComponent* UCancerTeamFunctionLibrary::K2_GetTeamManagerComponent(AActor* Actor, bool& bIsValid,
                                                                             TSubclassOf<UCancerTeamComponent>
                                                                             OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetTeamManagerComponent(Actor))
	{
		if (!IsValid(OutputClass))
		{
			OutputClass = UCancerTeamComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

FGameplayTag UCancerTeamFunctionLibrary::GetTeamTagForActor(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return FGameplayTag();
	}

	if (const UCancerTeamComponent* TeamComponent = Actor->FindComponentByClass<UCancerTeamComponent>())
	{
		return TeamComponent->TeamTag;
	}

	return FGameplayTag();
}

FGenericTeamId UCancerTeamFunctionLibrary::GetGenericTeamIdForActor(AActor* Actor)
{
	if (!Actor) return FGenericTeamId::NoTeam;
	if (const UCancerTeamComponent* TeamComp = Actor->FindComponentByClass<UCancerTeamComponent>())
	{
		return FGenericTeamId(TeamComp->GetTeamIndex(TeamComp->TeamTag));
	}
	return FGenericTeamId::NoTeam;
}

ETeamAttitude::Type UCancerTeamFunctionLibrary::GetTeamAttitude(AActor* SelfActor, AActor* Target)
{
	FGameplayTag SelfTeam = GetTeamTagForActor(SelfActor);
	FGameplayTag TargetTeam = GetTeamTagForActor(Target);
	if (SelfTeam.IsValid() && TargetTeam.IsValid())
	{
		if (auto TeamComponent = GetTeamManagerComponent(SelfActor))
		{
			return TeamComponent->GetAttitude(SelfTeam, TargetTeam);
		}
	}
	return ETeamAttitude::Neutral;
}

void UCancerTeamFunctionLibrary::SetSelfTeamTag(AActor* SelfActor, FGameplayTag TargetTeamTag)
{
	auto SelfTeamTag = GetTeamTagForActor(SelfActor);
	
	if (!TargetTeamTag.IsValid() || !SelfTeamTag.IsValid())
	{
		return ;
	}

	const FString TargetString = TargetTeamTag.ToString();
	TArray<FString> TargetParts;
	TargetString.ParseIntoArray(TargetParts, TEXT("."), true);

	if (TargetParts.Num() < 2)
	{
		return ;
	}

	FString ResultString = TargetParts[0] + TEXT(".") + TargetParts[1];

	const FString SelfString = SelfTeamTag.ToString();
	TArray<FString> SelfParts;
	SelfString.ParseIntoArray(SelfParts, TEXT("."), true);

	for (int32 i = 2; i < SelfParts.Num(); ++i)
	{
		ResultString += TEXT(".") + SelfParts[i];
	}

	if (auto TeamComponent = GetTeamManagerComponent(SelfActor))
	{
		TeamComponent->SetTeamTag(FGameplayTag::RequestGameplayTag(FName(*ResultString)));
	}
}
