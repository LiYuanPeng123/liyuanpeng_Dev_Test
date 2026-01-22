#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ComboInput.generated.h"

class UComboInputConfig;
class UComboInputMapping;

/**
 * 输入组件（独立）
 * - 负责物理→逻辑标签映射、方向派生、和弦/双击/长按识别
 * - 维护输入缓冲（时间窗与数量上限），输出解析后的标签集合
 * - 与增强输入适配器配合，但不依赖具体输入系统
 */
USTRUCT(BlueprintType)
struct FComboInputEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp = 0.f;
};

USTRUCT(BlueprintType)
struct FComboResolvedInput
{
	GENERATED_BODY()
public:
	/** 解析后的标签集合（行为/修饰/方向） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;
	/** 解析时间戳（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputResolved, const FComboResolvedInput&, Resolved);

UCLASS(ClassGroup=(Combo), meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UComboInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UComboInputComponent();

	/** 缓存按下的物理标签（自动映射为逻辑标签） */
	UFUNCTION(BlueprintCallable)
	void BufferPressTags(const FGameplayTagContainer& Tags);

	/** 缓存抬起的物理标签（记录释放时间以支持长按） */
	UFUNCTION(BlueprintCallable)
	void BufferReleaseTags(const FGameplayTagContainer& Tags);

	/** 获取解析后的输入缓冲（用于拉取模式） */
	UFUNCTION(BlueprintCallable)
	TArray<FComboResolvedInput> GetBufferedResolvedInputs() const;

	/** 清空解析后的输入缓冲 */
	UFUNCTION(BlueprintCallable)
	void ClearBuffer();

	/** 反馈轴数值（用于方向派生） */
	UFUNCTION(BlueprintCallable)
	void FeedAxis(const FGameplayTag& AxisTag, float Value);

	/** 输入解析配置（时间窗、阈值、上限） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboInputConfig> Config;

	/** 输入映射配置（物理→逻辑、和弦） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UComboInputMapping> Mapping;

	/** 解析回调（事件模式） */
	UPROPERTY(BlueprintAssignable)
	FOnInputResolved OnInputResolved;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<FComboInputEvent> RawInputs;

	UPROPERTY()
	TArray<FComboResolvedInput> ResolvedInputs;

	UPROPERTY()
	TMap<FGameplayTag, float> AxisValues;

	UPROPERTY()
	TMap<FGameplayTag, float> LastPressTime;

	UPROPERTY()
	TMap<FGameplayTag, float> LastReleaseTime;

	void PurgeOld();
	FGameplayTagContainer BuildDirectionalTags() const;
	void ResolveAndBuffer(const FGameplayTagContainer& LogicalPress);
};
