#include "CancerCharacterSubsystem.h"

void UCancerCharacterSubsystem::OnCharacterDead(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	SpawnedCharacters.Remove(Actor);
}

void UCancerCharacterSubsystem::AddCharacter(AActor* InNewCharacter)
{
	if (InNewCharacter == nullptr) return;
	SpawnedCharacters.AddUnique(InNewCharacter);
	InNewCharacter->OnEndPlay.AddDynamic(this, &ThisClass::OnCharacterDead);
}

TArray<AActor*> UCancerCharacterSubsystem::FindCharacterByQuery(const TArray<FCancerQueryMatch>& TagQuery)
{
	TArray<AActor*> Result;

	for (auto It = SpawnedCharacters.CreateIterator(); It; ++It)
	{
		if (AActor* AccessableActor = It->Get())
		{
			if (UCancerCoreFunctionLibrary::MatchAllQueryByActor(AccessableActor, TagQuery))
			{
				Result.Add(AccessableActor);
			}
		}
		else
		{
			It.RemoveCurrent();
		}
	}
	
	return Result;
}
