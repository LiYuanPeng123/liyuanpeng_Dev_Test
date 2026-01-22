#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MoveDefinition.generated.h"

class UAnimMontage;

/**
 * 招式段定义（数据资产）
 * - 动画：Montage/Section/PlayRate/RootMotionScale
 * - 条件：RequiredStateTags
 * - 输入：InputTags（触发该段的逻辑标签）
 * - 窗口：由动画通知状态驱动（不使用时间字段）
 */
UCLASS(BlueprintType)
class CANCERABILITY_API UMoveDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SectionName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RootMotionScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer RequiredStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer InputTags;

	UFUNCTION(BlueprintCallable)
	UAnimMontage* ResolveMontage() const;
};
