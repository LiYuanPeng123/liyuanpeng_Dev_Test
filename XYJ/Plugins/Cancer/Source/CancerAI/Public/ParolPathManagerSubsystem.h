#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ParolPathManagerSubsystem.generated.h"

struct FGameplayTag;
class APatrolPath;
/**
 * 
 */
UCLASS()
class CANCERAI_API UParolPathManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void AddPatrolPath(APatrolPath* PatrolPath);
	void RemovePatrolPath(APatrolPath* PatrolPath);
	void ClearPatrolPaths();
	TArray<class APatrolPath*> GetAllPatrolPathArray();
	UFUNCTION(BlueprintCallable, Category = "PatrolPath")
	class APatrolPath* GetPatrolPath(const FGameplayTag& InTag);

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<class APatrolPath*> PatrolPathArray;
};
