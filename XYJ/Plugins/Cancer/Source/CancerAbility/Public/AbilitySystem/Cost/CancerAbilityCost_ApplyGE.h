// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "AbilitySystem/Cost/CancerAbilityCost.h"
#include "CancerAbilityCost_ApplyGE.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ECheckerType:uint8
{
	Equal UMETA(DisplayName = "等于"),
	NotEqual  UMETA(DisplayName = "不等于"),
	Greater  UMETA(DisplayName = "大于"),
	GreaterOrEqual  UMETA(DisplayName = "大于等于"),
	Less  UMETA(DisplayName = "小于"),
	LessOrEqual  UMETA(DisplayName = "小于等于")
};

UCLASS()
class CANCERABILITY_API UCancerAbilityCost_ApplyGE : public UCancerAbilityCost
{
	GENERATED_BODY()
public:
	virtual bool CheckCost(const UCancerGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const;
	
	virtual void ApplyCost(const UCancerGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);
	
	
	// 应用的GE
	UPROPERTY(EditAnywhere,  Category=Costs, DisplayName="应用GE")
	TSubclassOf<UGameplayEffect> ApplyGE;
	
	// 检测的属性
	UPROPERTY(EditAnywhere,  Category=Costs, DisplayName="属性")
	FGameplayAttribute CheckAttribute;

	// 基准属性(可选)。
	// 如果设置，"检测值"将作为系数乘以该属性的值作为最终阈值。
	// 例如：属性选CurrentHP，基准属性选MaxHP，检测值填0.3，则表示检测 CurrentHP < 0.3 * MaxHP
	UPROPERTY(EditAnywhere, Category = Costs, DisplayName = "基准属性(可选)")
	FGameplayAttribute ReferenceAttribute;
	
	// 检测方式
	UPROPERTY(EditAnywhere,  Category=Costs, DisplayName="检测方式")
	ECheckerType CheckType;
	
	// 检测值
	UPROPERTY(EditAnywhere,  Category=Costs, DisplayName="检测值")
	FScalableFloat CheckValue;

	// 如果设置，会将检测值通过SetByCaller注入到GE中，实现“检测值即消耗值”
	UPROPERTY(EditAnywhere, Category = Costs, DisplayName = "注入数值Tag")
	FGameplayTag InjectionTag;

	// 注入给GE的实际消耗值。如果不填，默认使用检测值。
	// 这允许处理“检测血量<30%，但消耗100怒气”这类检测阈值与消耗值分离的情况。
	UPROPERTY(EditAnywhere, Category = Costs, DisplayName = "消耗值(可选)")
	FScalableFloat CostValue;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	// 生成消耗描述文本，供UI显示（例如："30 法力"）
	virtual FString GetDescription(int32 Level) const;
};
