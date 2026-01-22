// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CancerStaticsSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FStaticsInfo
{
	GENERATED_BODY()
	
	// 伤害
	UPROPERTY(BlueprintReadWrite,EditAnywhere,DisplayName="总伤害")
	float TotalDamage {0.f} ;
	
	// 硬直
	UPROPERTY(BlueprintReadWrite,EditAnywhere,DisplayName="硬直次数统计")
	int32 TotalStagger {0} ;
	
	// 格挡
	UPROPERTY(BlueprintReadWrite,EditAnywhere,DisplayName="连续格挡")
	int32 TotalBlock {0};
	
	// 弹反
	UPROPERTY(BlueprintReadWrite,EditAnywhere,DisplayName="完美闪避(弹反)")
	int32 TotalPerfectBlock {0} ;
	
	//开始战斗的时间  GetWorld()->GetUnpausedTimeSeconds()
	// 非战斗状态 设置为-1
	UPROPERTY(BlueprintReadWrite,EditAnywhere,DisplayName="战斗时长")
	double BattleTime {0.f};
	
	FTimerHandle BattleTimerHandle;
	
};

/**
 * 用于统计各种数值的子系统
 */
UCLASS()
class CANCERABILITY_API UCancerStaticsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	// 复位
	void Reset();
	
	// 统计怪物受到的伤害
	UFUNCTION(BlueprintCallable)
	static void StaticDamage(AActor* Target, float InDamage);
	
	// 获取指定角色的总伤害
	UFUNCTION(BlueprintCallable)
	float GetTotalDamage(AActor* Target);
	
	// 统计怪物受到的硬直
	UFUNCTION(BlueprintCallable)
	static void StaticStagger(AActor* Target);
	
	// 获取指定角色的总硬直
	UFUNCTION(BlueprintCallable)
	int32 GetTotalStagger(AActor* Target);
	
	// 统计连续格挡
	UFUNCTION(BlueprintCallable)
	static void StaticBlock(AActor* Target);
	
	UFUNCTION(BlueprintCallable)
	int32 GetTotalBlock(AActor* Target);
	
	// 统计怪物被弹反
	UFUNCTION(BlueprintCallable)
	static void StaticPerfectBlock(AActor* Target);
	
	UFUNCTION(BlueprintCallable)
	int32 GetTotalPerfectBlock(AActor* Target);
	
	// 战斗计时
	UFUNCTION(BlueprintCallable)
	static void StaticBattleStart(AActor* Target);
	
	// 战斗计时结束
	UFUNCTION(BlueprintCallable)
	static void StaticBattleEnd(AActor* Target);
	
	UFUNCTION(BlueprintCallable)
	double GetTotalBattleTime(AActor* Target);
	
	// 统计信息
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<AActor* , FStaticsInfo> CharacterStaticsInfo;
	
	FTimerHandle DamageStaticsTimerHandle;
	FTimerHandle StaggerStaticsTimerHandle;
	FTimerHandle BlockStaticsTimerHandle;
	FTimerHandle PerfectBlockStaticsTimerHandle;
};
