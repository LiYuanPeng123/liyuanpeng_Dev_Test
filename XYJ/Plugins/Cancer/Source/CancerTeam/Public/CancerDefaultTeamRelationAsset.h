// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamCoreType.h"
#include "Engine/DataAsset.h"
#include "CancerDefaultTeamRelationAsset.generated.h"

/**
 *  关卡中整体阵营关系
 */
UCLASS()
class CANCERTEAM_API UCancerDefaultTeamRelationAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 队伍之间的相对关系
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TMap<FGameplayTag, FTeamInfo> TeamInfo; 
	
	// 阵营标签到整型 Id 的稳定映射
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cancer, DisplayName="阵营索引映射")
	TMap<FGameplayTag, uint8> TeamIndexMap;
};
