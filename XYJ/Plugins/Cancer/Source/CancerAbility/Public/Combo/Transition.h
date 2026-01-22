#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Transition.generated.h"

class UMoveDefinition;

/**
 * 过渡（当前段 → 目标段）
 * - InputTags：匹配输入标签（行为/方向/修饰）
 * - RequiredStateTags：状态约束（空中/地面/锁定等）
 * - 距离/角度：上下文约束（可选）
 * - Priority：数值越小优先级越高
 */
USTRUCT(BlueprintType)
struct FTransition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMoveDefinition> TargetMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer InputTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinAngleDeg = -180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAngleDeg = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 100;
};

USTRUCT(BlueprintType)
struct FCancerTransitionList
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTransition> Items;
};

