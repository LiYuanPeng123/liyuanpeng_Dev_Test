#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ComboPreInputArbitrationConfig.generated.h"

/**
 * 预输入仲裁配置
 * - 定义预输入在窗口开启时的选择策略（时间规则、手势权重、分类标签）
 * - 通过可配置参数适配不同战斗风格（例如取消优先、攻击优先、容错时间等）
 */
UENUM(BlueprintType)
enum class EComboPreInputTimePolicy : uint8
{
	/** 后按优先：更符合“最后意图覆盖”的直觉 */
	LastPressWins,
	/** 先按优先：更保守，保证较早输入的稳定性 */
	FirstPressWins
};

UENUM(BlueprintType)
enum class EComboWindowType : uint8
{
	/** 连招窗口：攻击派生优先 */
	Combo,
	/** 取消窗口：闪避/格挡等取消优先 */
	Cancel
};

UENUM(BlueprintType)
enum class EComboSimultaneousPriority : uint8
{
	/** 同时开启时先消费取消，再消费连招 */
	CancelFirst,
	/** 同时开启时先消费连招，再消费取消 */
	ComboFirst
};

UCLASS(BlueprintType)
class CANCERABILITY_API UComboPreInputArbitrationConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 时间策略：选择更贴近设计预期的“先/后按优先” */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EComboPreInputTimePolicy TimePolicy = EComboPreInputTimePolicy::LastPressWins;

	/** 预输入最大保留时长（秒），超过则丢弃以避免旧输入污染 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxPreInputAgeSeconds = 0.4f;

	/** 每个分桶（攻击/取消）保留的最大预输入条数 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxPreInputCountPerBucket = 4;

	/** 和弦权重（方向+输入同时存在时加分，越大越优先） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ChordWeight = 3;

	/** 长按权重（持续按压输入加分，越大越优先） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 HoldWeight = 2;

	/** 双击权重（快速两次按压加分，越大越优先） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DoubleTapWeight = 1;

	/** 取消类标签集合（例如格挡/闪避），用于分类到取消分桶 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer CancelInputTags;

	/** 攻击类标签集合（例如轻/重攻击），用于分类到攻击分桶 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer AttackInputTags;

	/** 同时开启 Combo 与 Cancel 窗口时的消费优先级 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EComboSimultaneousPriority SimultaneousPriority = EComboSimultaneousPriority::CancelFirst;
};
