// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerAISetting.generated.h"

UCLASS(config = Game, DefaultConfig,DisplayName="CancerAI")
class CANCERAI_API UCancerAISetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerAISetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
