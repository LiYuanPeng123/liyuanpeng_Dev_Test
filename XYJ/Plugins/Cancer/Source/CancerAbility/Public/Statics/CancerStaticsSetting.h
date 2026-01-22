// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerStaticsSetting.generated.h"

/**
 * 统计信息的一些设置
 */
UCLASS()
class CANCERABILITY_API UCancerStaticsSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	// 伤害统计的重置时间
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DamageResetTime{3.f};
	
	// 硬直统计的重置时间
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float StaggerResetTime{3.f};
	
	// 格挡统计的重置时间
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BlockResetTime{3.f};
	
	// 弹反统计的重置时间
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float PerfectBlockResetTime{3.f};
	
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
