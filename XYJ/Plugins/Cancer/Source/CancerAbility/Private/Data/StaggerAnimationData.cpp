#include "Data/StaggerAnimationData.h"

#include "Cooker/CookDependency.h"

FStaggerAnimationConfig UStaggerAnimationDataAsset::FindClosestAnimation(const TArray<FStaggerAnimationConfig> Array,
                                                                         int32 AnimID) const
{
	if (Array.Num() == 0)
	{
		return FStaggerAnimationConfig();
	}

	const FStaggerAnimationConfig* ClosestConfig = nullptr;

	int32 BestDelta = TNumericLimits<int32>::Max();

	for (const FStaggerAnimationConfig& Config : Array)
	{
		// 先处理精确匹配
		if (Config.StaggerMontageID == AnimID)
		{
			return Config;
		}

		const int32 Delta = FMath::Abs(Config.StaggerMontageID - AnimID);
		if (Delta < BestDelta)
		{
			BestDelta = Delta;
			ClosestConfig = &Config;
		}
		else if (Delta == BestDelta && ClosestConfig)
		{
			// 当差值相同，优先选择编号更小的
			if (Config.StaggerMontageID < ClosestConfig->StaggerMontageID)
			{
				ClosestConfig = &Config;
			}
		}
	}

	return ClosestConfig ? *ClosestConfig : FStaggerAnimationConfig();
}

FStaggerAnimationConfig UStaggerAnimationDataAsset::FindStaggerAnimation(
	EStaggerType Module, int32 AnimationID, bool bExactMatch) const
{
	TArray<FStaggerAnimationConfig> TargetArray;

	switch (Module)
	{
	case EStaggerType::Normal:
		TargetArray = NormalAnimations;
		break;
	case EStaggerType::Air:
		TargetArray = AirAnimations;
		break;
	case EStaggerType::KnockDown:
		TargetArray = KnockDownAnimations;
		break;
	case EStaggerType::KnockFly:
		TargetArray = KnockFlyAnimations;
		break;
	}

	if (TargetArray.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No Stagger Animation Data"));
		return FStaggerAnimationConfig();
	}

	for (const FStaggerAnimationConfig& Config : TargetArray)
	{
		if (Config.StaggerMontageID == AnimationID)
		{
			return Config;
		}
	}

	// 如果要求精确匹配但没找到，返回nullptr
	if (bExactMatch)
	{
		UE_LOG(LogTemp, Error, TEXT("No Stagger Animation Data ID:[%s,%d]"),
		       *StaticEnum<EStaggerType>()->GetNameStringByValue((int64)Module), AnimationID);
		return FStaggerAnimationConfig();
	}

	// 空中状态特殊逻辑：向下取最近的ID
	if (Module == EStaggerType::Air)
	{
		const FStaggerAnimationConfig* BestConfig = nullptr;
		
		for (const FStaggerAnimationConfig& Config : TargetArray)
		{
			// 必须比目标ID小
			if (Config.StaggerMontageID < AnimationID)
			{
				// 如果还没找到过，或者找到了更大的ID（更接近目标ID），则更新
				if (BestConfig == nullptr || Config.StaggerMontageID > BestConfig->StaggerMontageID)
				{
					BestConfig = &Config;
				}
			}
		}

		if (BestConfig)
		{
			return *BestConfig;
		}

		// A) 如果向下读取也读取不到，则不播放硬直动画
		return FStaggerAnimationConfig();
	}

	// 对于非精确匹配（倒地连和浮空连），查找最接近的编号
	return FindClosestAnimation(TargetArray, AnimationID);
}

const TArray<FStaggerAnimationConfig>& UStaggerAnimationDataAsset::GetAnimationsByModule(
	EStaggerType Module) const
{
	switch (Module)
	{
	case EStaggerType::Normal:
		return NormalAnimations;
	case EStaggerType::KnockDown:
		return KnockDownAnimations;
	case EStaggerType::KnockFly:
		return KnockFlyAnimations;
	case EStaggerType::Air:
		return AirAnimations;
	default:
		static TArray<FStaggerAnimationConfig> EmptyArray;
		return EmptyArray;
	}
}
