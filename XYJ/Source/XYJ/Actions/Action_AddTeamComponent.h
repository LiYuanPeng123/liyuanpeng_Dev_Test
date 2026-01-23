#pragma once
#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddTeamComponent.generated.h"

class UTeamConfigDataAsset;

UCLASS(DisplayName="团队组件")
class XYJ_API UAction_AddTeamComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  DisplayName="阵营配置")
	FGameplayTag Team;

protected:
	virtual void PostInitComponent_Implementation() override;
};
