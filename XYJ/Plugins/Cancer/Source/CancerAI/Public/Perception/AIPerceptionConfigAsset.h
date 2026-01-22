#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIPerceptionConfigAsset.generated.h"

class UBehaviorTree;
class UAISenseConfig;
/**
 */
UCLASS(BlueprintType)
class CANCERAI_API UAIPerceptionConfigAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 感知配置（视觉/听觉等）
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "AI Perception")
	TArray<TObjectPtr<UAISenseConfig>> SensesConfigs;

	// 阵营过滤开关
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Affiliation")
	bool bDetectEnemies = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Affiliation")
	bool bDetectNeutrals = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Affiliation")
	bool bDetectFriendlies = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TagFilter|ActorTags")
	TArray<FName> RequiredActorTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TagFilter|ActorTags")
	TArray<FName> BlockedActorTags;

	void ApplyToController(class AAIController* Controller) const;

	bool ShouldAcceptTarget(const class AAIController* Controller, const class AActor* Target) const;
};
