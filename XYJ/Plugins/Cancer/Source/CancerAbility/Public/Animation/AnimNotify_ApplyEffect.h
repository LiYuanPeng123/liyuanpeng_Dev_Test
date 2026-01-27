// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotify.h"
#include "AnimNotify_ApplyEffect.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS(DisplayName="应用GE(持续时间)")
class CANCERABILITY_API UAnimNotify_ApplyEffect : public UCancerCombatAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString DisplayName =TEXT("");
	
	// 要施加的GameplayEffect类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="游戏效果", Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	// GE的等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="效果等级", Category = "GameplayEffect",
		meta = (ClampMin = "0"))
	int32 EffectLevel = 1;

	// 效果持续时间（如果GE设置为HasDuration）。若<=0则使用GE默认配置。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="持续时间", Category = "GameplayEffect")
	float Duration = -1.0f;
};
