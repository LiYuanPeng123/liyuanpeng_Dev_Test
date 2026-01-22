
#pragma once

#include "Engine/StreamableManager.h"

DECLARE_LOG_CATEGORY_EXTERN(AssetManagerStartupJobLog,Log,All)

// 声明一个委托，用于报告子步骤的进度更新，参数为新的进度值(0.0-1.0)
DECLARE_DELEGATE_OneParam(FXGAssetManagerStartupJobSubstepProgress, float /*NewProgress*/);

/**
 * 资源管理器启动作业结构体
 * 用于管理异步资源加载任务，支持进度报告和权重控制
 */
struct FAssetManagerStartupJob
{
	// 子步骤进度更新委托
	FXGAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
	
	// 作业函数，实际执行资源加载的回调函数
	TFunction<void(const FAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;
	
	// 作业名称，用于标识和调试
	FString JobName;
	
	// 作业权重，用于计算整体进度
	float JobWeight;
	
	// 上次更新时间戳，用于限制更新频率
	mutable double LastUpdate = 0;

	/**
	 * 构造函数
	 * @param InJobName 作业名称
	 * @param InJobFunc 要执行的回调函数
	 * @param InJobWeight 作业在整体进度中的权重
	 */
	FAssetManagerStartupJob(const FString& InJobName, 
		const TFunction<void(const FAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, 
		float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{}

	/**
	 * 执行作业
	 * @return 返回可流式加载的句柄，如果作业是同步的则返回nullptr
	 */
	TSharedPtr<FStreamableHandle> DoJob() const;

	/**
	 * 更新子步骤进度
	 * @param NewProgress 新的进度值(0.0-1.0)
	 */
	void UpdateSubstepProgress(float NewProgress) const
	{
		// 如果绑定了委托，则调用委托更新进度
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}

	/**
	 * 从可流式句柄更新子步骤进度
	 * @param StreamableHandle 可流式加载句柄
	 */
	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		// 只有在绑定了委托的情况下才更新进度
		if (SubstepProgressDelegate.IsBound())
		{
			// 获取当前时间戳
			// 注意：StreamableHandle::GetProgress() 会遍历大型图结构，性能开销较大
			double Now = FPlatformTime::Seconds();
			// 限制更新频率，每秒最多更新60次
			if (Now - LastUpdate > 1.0 / 60)
			{
				// 执行委托，传入当前加载进度
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};