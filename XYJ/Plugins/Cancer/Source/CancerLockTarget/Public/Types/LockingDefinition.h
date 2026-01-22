
#pragma once

#include "CoreMinimal.h"

#include "LockingDefinition.generated.h"

USTRUCT(BlueprintType)
struct FLockingConfigInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="最大锁定距离"))
	float MaxLockingDistance = 2000;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="锁定2D屏幕最大距离(百分比)"))
	float MaxLocking2DDistance = 80;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="锁定空间距离权重"))
	float Locking3DDistanceWeight = 1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="锁定2D屏幕距离权重"))
	float Locking2DDistanceWeight = 3;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="操作权重"))
	float LockingOperationWeight = 1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="威胁权重"))
	float LockingThreatenWeight = 1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="最大切锁距离"))
	float MaxChangeLockDistance = 1000;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="最大切锁方向偏移夹角"))
	float MaxChangeLockAngle = 90;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="切锁距离权重"))
	float ChangeLockWeight = 1;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="切锁方向偏移权重"))
	float ChangeLockAngleWeight = 1;
};


USTRUCT(BlueprintType)
struct FFindLockingPointInfo
{
	GENERATED_USTRUCT_BODY()

	//锁定点
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="锁定点"))
	TObjectPtr<class UCancerLockingPointComponent> LockingPointComponent;
	//距离
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="距离"))
	float Distance = 0.0f;
	//控制器夹角
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="控制器夹角"))
	FVector2D ControllerIncludedAngle = FVector2D::ZeroVector;
	//角色夹角
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="角色夹角"))
	FVector2D CharacterIncludedAngle = FVector2D::ZeroVector;
	//与屏幕中心点距离
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="屏幕2D坐标"))
	FVector2D Screen2DPoint = FVector2D::ZeroVector;
	//与屏幕中心点距离
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="与屏幕中心点距离"))
	float ScreenPointDistance = 0.0f;
	//方向投影距离X
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="方向投影距离X"))
	float DirectionProjectionDistanceX = 0.0f;
	//方向投影距离Y
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="方向投影距离Y"))
	float DirectionProjectionDistanceY = 0.0f;
	//方向投影距离
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="方向投影距离"))
	float DirectionProjectionDistance = 0.0f;
	//与锁定点夹角
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="与锁定点夹角"))
	float LockingIncludedAngle = 0.0f;
	//与锁定点距离
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="与锁定点距离"))
	float LockingIncludedDistance = 0.0f;
	//方向投影距离权重
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="方向投影距离权重"))
	float DirectionProjectionDistanceWeights = 0.0f;
	//与锁定点夹角权重
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="与锁定点夹角权重"))
	float LockingIncludedAngleWeights = 0.0f;
	//与锁定点距离权重
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="与锁定点距离权重"))
	float LockingIncludedDistanceWeights = 0.0f;
	//总权重
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="总权重"))
	float TotalWeights = 0.0f;
};