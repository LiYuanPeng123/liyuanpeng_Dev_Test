#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "AN_BlendByGameplayTag.generated.h"

/**
 * 根据游戏标签混合动画节点
 * 
 * 该节点用于根据游戏标签选择和混合动画
 * 主要功能：
 * - 根据激活的游戏标签选择对应的动画
 * - 支持多个标签的配置
 * - 可以在编辑器中设置标签
 * 
 * 使用方法：
 * 1. 在编辑器中设置需要使用的标签
 * 2. 将动画序列添加到节点的输入
 * 3. 根据激活的标签自动选择对应的动画
 */
USTRUCT(DisplayName="Blend By Tag")
struct CANCERANIMATION_API FAN_BlendByGameplayTag : public FAnimNode_BlendListBase
{
	GENERATED_BODY()
public:
	// 编辑器数据
#if WITH_EDITORONLY_DATA
	/**
	 * 当前激活的标签
	 * 
	 * 该标签用于确定当前应该播放哪个动画
	 * 在运行时会根据实际激活的标签自动更新
	 */
	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty, PinShownByDefault))
	FGameplayTag ActiveTag;
    
	/**
	 * 可用的标签列表
	 * 
	 * 该数组包含所有可以用于选择动画的标签
	 * 每个标签对应一个动画序列
	 * 在编辑器中可以添加和删除标签
	 */
	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty))
	TArray<FGameplayTag> Tags;
#endif
    
protected:
	/**
	 * 获取当前激活的子节点索引
	 * 
	 * 该函数根据激活的标签返回对应的动画序列索引
	 * 如果没有匹配的标签，则返回0
	 */
	virtual int32 GetActiveChildIndex() override;
    
public:
	/**
	 * 获取当前激活的标签
	 * 
	 * @return 当前激活的标签
	 */
	const FGameplayTag& GetActiveTag() const;
    
	/**
	 * 获取所有可用的标签列表
	 * 
	 * @return 标签列表
	 */
	const TArray<FGameplayTag>& GetTags() const;
    
#if WITH_EDITOR
	/**
	 * 刷新姿势引脚
	 * 
	 * 该函数在编辑器中用于更新节点的引脚显示
	 * 会根据标签列表自动调整引脚数量
	 */
	void RefreshPosePins();
#endif
};
