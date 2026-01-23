#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddAIPerceptionComponent.generated.h"

class UAIPerceptionConfigAsset;
/**
 * 
 */
UCLASS(DisplayName="AI感知组件")
class XYJ_API UAction_AddAIPerceptionComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI行为树")
	TSoftObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings", DisplayName = "感知配置")
	TSoftObjectPtr<UAIPerceptionConfigAsset> PerceptionConfig;

protected:
	virtual void PostInitComponent_Implementation() override;

	virtual void FinalizeAfterComponent_Implementation() override;
};
