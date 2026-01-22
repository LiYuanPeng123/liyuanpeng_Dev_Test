

#include "AssetManagerStartupJob.h"

DEFINE_LOG_CATEGORY(AssetManagerStartupJobLog)

UE_DISABLE_OPTIMIZATION
/**
 * 执行启动作业
 * @return 返回可流式加载的句柄，如果作业是同步的则返回nullptr
 */
TSharedPtr<FStreamableHandle> FAssetManagerStartupJob::DoJob() const
{
	// 记录作业开始时间，用于计算执行耗时
	const double JobStartTime = FPlatformTime::Seconds();

	// 声明可流式加载句柄
	TSharedPtr<FStreamableHandle> Handle;
	
	// 记录作业开始日志
	UE_LOG(AssetManagerStartupJobLog, Display, TEXT("开始执行启动作业 \"%s\""), *JobName);
	
	// 执行实际的作业函数
	JobFunc(*this, Handle);

	// 如果作业返回了有效的句柄，说明是异步加载任务
	if (Handle.IsValid())
	{
		// 绑定更新委托，当资源加载进度更新时调用
		// 注意：这会覆盖任何已绑定的委托
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(
			this, &FAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));

		/**
		 * 阻塞当前线程，直到请求的资源加载完成
		 * 这会将请求的资源推送到加载队列的顶部，
		 * 但不会刷新所有异步加载，通常比直接调用 LoadObject 更快
		 *
		 * @param Timeout 最大等待时间（秒），0 表示无限等待
		 * @param bStartStalledHandles 如果为 true，将强制所有等待外部资源的句柄立即尝试加载
		 */
		Handle->WaitUntilComplete(0.0f, false);
		
		// 清除更新委托，避免后续的进度更新
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	// 记录作业完成日志，包含执行耗时
	UE_LOG(AssetManagerStartupJobLog, Display, 
		TEXT("启动作业 \"%s\" 执行完成，耗时 %.2f 秒"), 
		*JobName, 
		FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
UE_ENABLE_OPTIMIZATION