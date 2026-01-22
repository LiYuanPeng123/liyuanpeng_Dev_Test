#pragma once

#include "CoreMinimal.h"
#include "CancerCombatAnimNotifyState.h"
#include "GameplayEffect.h"
#include "AnimNotifyState_ApplyGameplayEffect.generated.h"

/**
 * 可配置施加GameplayEffect的动画通知状态
 * 支持配置具体的GE和GE等级
 */
UCLASS(DisplayName="施加游戏效果")
class CANCERABILITY_API UAnimNotifyState_ApplyGameplayEffect : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()

public:
	// 要施加的GameplayEffect类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="游戏效果", Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	// GE的等级
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="效果等级", Category = "GameplayEffect", meta = (ClampMin = "0"))
	int32 EffectLevel = 1;

	// 效果持续时间（如果GE设置为HasDuration）。若<=0则使用GE默认配置。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="持续时间", Category = "GameplayEffect")
	float Duration = -1.0f;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	// 存储应用的效果句柄，用于在结束时移除
	UPROPERTY()
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, FActiveGameplayEffectHandle> ActiveEffectHandles;
};