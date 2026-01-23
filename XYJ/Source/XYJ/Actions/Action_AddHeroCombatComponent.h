
#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddHeroCombatComponent.generated.h"

UCLASS(DisplayName="角色战斗组件")
class XYJ_API UAction_AddHeroCombatComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= Widget)
	TSoftClassPtr<UUserWidget> BossWidgetClass;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= Widget)
	bool bIsShowBossWidget = true;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= Widget)
	FGameplayTag SlotID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="进入状态标签")
	FGameplayTagContainer TriggerTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="退出状态标签")
	FGameplayTagContainer ExitTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="战斗状态标签")
	 FGameplayTag ComboTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="退出时间")
	float ExitCombatTime = 5.f;
	
	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;
	
};
