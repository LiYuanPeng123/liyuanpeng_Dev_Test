
#include "Nodes/AN_BlendByGameplayTag.h"
/**
 * 获取当前激活的子节点索引
 * 
 * 该函数根据当前激活的标签返回对应的动画序列索引
 * 主要步骤：
 * 1. 获取当前激活的标签
 * 2. 在标签列表中查找该标签
 * 3. 返回找到的位置（索引+1）
 * 
 * @return 子节点索引（0表示未找到匹配的标签）
 */
int32 FAN_BlendByGameplayTag::GetActiveChildIndex()
{
	const auto& CurrentActiveTag{GetActiveTag()};
	return CurrentActiveTag.IsValid()
			   ? GetTags().Find(CurrentActiveTag) + 1
			   : 0;
}

/**
 * 获取当前激活的标签
 * 
 * 该函数从动画节点数据中获取当前激活的标签
 * 
 * @return 当前激活的标签
 */
const FGameplayTag& FAN_BlendByGameplayTag::GetActiveTag() const
{
	return GET_ANIM_NODE_DATA(FGameplayTag, ActiveTag);
}

/**
 * 获取所有可用的标签列表
 * 
 * 该函数从动画节点数据中获取所有可用的标签列表
 * 
 * @return 标签列表
 */
const TArray<FGameplayTag>& FAN_BlendByGameplayTag::GetTags() const
{
	return GET_ANIM_NODE_DATA(TArray<FGameplayTag>, Tags);
}

#if WITH_EDITOR
/**
 * 刷新姿势引脚
 * 
 * 该函数用于在编辑器中同步姿势引脚数量和标签数量
 * 主要步骤：
 * 1. 计算姿势引脚数量和标签数量的差异
 * 2. 如果引脚数量过多，删除多余的引脚
 * 3. 如果引脚数量不足，添加新的引脚
 * 
 * @note 引脚数量始终比标签数量多1个，因为第一个引脚用于默认动画
 */
void FAN_BlendByGameplayTag::RefreshPosePins()
{
	// 计算引脚数量和标签数量的差异
	const auto Difference{BlendPose.Num() - GetTags().Num() - 1};
	if (Difference == 0)
	{
		return;
	}

	// 如果引脚数量过多，删除多余的引脚
	if (Difference > 0)
	{
		for (auto i{Difference}; i > 0; i--)
		{
			RemovePose(BlendPose.Num() - 1);
		}
	}
	// 如果引脚数量不足，添加新的引脚
	else
	{
		for (auto i{Difference}; i < 0; i++)
		{
			AddPose();
		}
	}
}
#endif