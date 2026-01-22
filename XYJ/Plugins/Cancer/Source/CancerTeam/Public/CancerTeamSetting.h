// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CancerDefaultTeamRelationAsset.h"
#include "Engine/DeveloperSettings.h"
#include "CancerTeamSetting.generated.h"

UCLASS(Config=Game ,DefaultConfig,DisplayName="团队设置")
class CANCERTEAM_API UCancerTeamSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerTeamSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
	
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "TeamSettings")
	TSoftObjectPtr<UCancerDefaultTeamRelationAsset> DefaultTeamData;
};
