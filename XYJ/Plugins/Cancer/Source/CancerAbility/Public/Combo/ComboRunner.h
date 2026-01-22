#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Combo/ComboInput.h"
#include "ComboRunner.generated.h"

class UComboGraph;
class UComboController;
class UComboInputComponent;
class UComboMontagePlayer;
class UMoveDefinition;

/**
 * 连招运行器（整合层）
 * - 自动查找/创建控制器、输入组件、蒙太奇播放器
 * - 订阅输入解析事件，在窗口期间驱动 BufferInput + TryAdvance
 * - 播放招式段动画，窗口由动画通知状态控制
 */
UCLASS(ClassGroup=(Combo), meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UComboRunner : public UActorComponent
{
	GENERATED_BODY()
public:
	UComboRunner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboGraph> Graph;

	UFUNCTION(BlueprintCallable)
	void Initialize(UComboGraph* InGraph);

	/** 启动连招（播放起始段动画） */
	UFUNCTION(BlueprintCallable)
	void Start();

	/** 外部喂入已解析输入（可与 OnInputResolved 并存） */
	UFUNCTION(BlueprintCallable)
	void FeedInput(const FGameplayTagContainer& InputTags);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UComboController* Controller;
	UPROPERTY()
	UComboInputComponent* Input;
	UPROPERTY()
	UComboMontagePlayer* Player;

	UFUNCTION()
	void HandleMoveStarted(UMoveDefinition* Move);
	UFUNCTION()
	void HandleMoveEnded(UMoveDefinition* Move);
	/** 输入解析事件回调：在窗口期间尝试派生 */
	UFUNCTION()
	void HandleInputResolved(const FComboResolvedInput& Resolved);
};
