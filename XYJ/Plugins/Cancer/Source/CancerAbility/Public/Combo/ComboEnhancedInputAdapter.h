#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ComboEnhancedInputAdapter.generated.h"

class UComboInputComponent;
class UComboInputMapping;
class UComboInputConfig;
class UComboEnhancedInputBindings;
class UEnhancedInputComponent;
struct FInputActionInstance;

/**
 * 增强输入适配器
 * - 将 IMC/IA 的 Started/Triggered/Completed 事件转发到输入组件
 * - 支持轴输入反馈与物理标签配置（资产化）
 * - 不与项目现有系统耦合，纯 Combo 内部使用
 */
UCLASS(ClassGroup=(Combo), meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UComboEnhancedInputAdapter : public UActorComponent
{
	GENERATED_BODY()
public:
	UComboEnhancedInputAdapter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboInputComponent> InputComponentRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboInputMapping> Mapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboInputConfig> Config;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboEnhancedInputBindings> Bindings;

	/** 初始化适配器（添加 IMC 与绑定 IA 回调） */
	UFUNCTION(BlueprintCallable)
	void InitializeAdapter();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComp;

	void BindActions();

	/** IA Started：缓存按下与轴值 */
	UFUNCTION()
	void OnActionStarted(const FInputActionInstance& Instance);
	/** IA Triggered：轴值持续反馈 */
	UFUNCTION()
	void OnActionTriggered(const FInputActionInstance& Instance);
	/** IA Completed：缓存释放 */
	UFUNCTION()
	void OnActionCompleted(const FInputActionInstance& Instance);
};

