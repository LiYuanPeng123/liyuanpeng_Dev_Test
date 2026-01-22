#include "ParolPathManagerSubsystem.h"
#include "PatrolPath.h"

void UParolPathManagerSubsystem::AddPatrolPath(APatrolPath* PatrolPath)
{
	PatrolPathArray.AddUnique(PatrolPath);
}

void UParolPathManagerSubsystem::RemovePatrolPath(APatrolPath* PatrolPath)
{
	PatrolPathArray.Remove(PatrolPath);
}

void UParolPathManagerSubsystem::ClearPatrolPaths()
{
	PatrolPathArray.Empty();
}

class APatrolPath* UParolPathManagerSubsystem::GetPatrolPath(const FGameplayTag& InTag)
{
	for (auto PatrolPath : PatrolPathArray)
	{
		if (PatrolPath->PathTag == InTag)
		{
			return PatrolPath;
		}
	}
	return nullptr;
}

TArray<class APatrolPath*> UParolPathManagerSubsystem::GetAllPatrolPathArray()
{
	return PatrolPathArray;
}
