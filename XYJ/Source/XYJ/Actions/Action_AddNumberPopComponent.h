// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddNumberPopComponent.generated.h"

class UCancerDamagePopStyleNiagara;
/**
 * 
 */
UCLASS(DisplayName="伤害数字组件")
class XYJ_API UAction_AddNumberPopComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Number Pop", DisplayName="风格设置")
	TSoftObjectPtr<UCancerDamagePopStyleNiagara> Style;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Number Pop", DisplayName="显示伤害")
	bool bIsShowDamage = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Number Pop", DisplayName="显示治疗")
	bool bIsShowCue = false;


	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;
};
