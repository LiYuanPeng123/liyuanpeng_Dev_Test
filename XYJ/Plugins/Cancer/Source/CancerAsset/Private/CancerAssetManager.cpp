#include "CancerAssetManager.h"

#include "CancerAssetSetting.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/AssetManagerSettings.h"

DEFINE_LOG_CATEGORY(CancerAssetManagerLog)

UE_DISABLE_OPTIMIZATION
//////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("Cancer.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UCancerAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

//

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) \
StartupJobs.Add(FAssetManagerStartupJob(#JobFunc, \
[this](const FAssetManagerStartupJob& StartupJob, \
TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))

#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

/*// 使用
STARTUP_JOB_WEIGHTED(LoadGameAssets(), 0.5f);

// 展开后
StartupJobs.Add(FCancerAssetManagerStartupJob(
	"LoadGameAssets",  // 使用函数名作为作业名
	[this](const FCancerAssetManagerStartupJob& StartupJob, 
		   TSharedPtr<FStreamableHandle>& LoadHandle) {
		LoadGameAssets();  // 执行传入的函数
	}, 
	0.5f  // 权重
));*/

//////////////////////////////////////////////////////////////////////


// 示例
/*1. 加载UI资源
{
   auto LoadUIAssets = [](const FCancerAssetManagerStartupJob& Job, TSharedPtr<FStreamableHandle>& OutHandle)
   {
	   TArray<FSoftObjectPath> UIAssets = {
		   FSoftObjectPath(TEXT("/Game/UI/HUD/MainHUD.MainHUD")),
		   FSoftObjectPath(TEXT("/Game/UI/Menus/MainMenu.MainMenu")),
		   // 添加更多UI资源...
	   };
		   
	   FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	   OutHandle = StreamableManager.RequestAsyncLoad(UIAssets);
   };
	   
   FCancerAssetManagerStartupJob UIAssetsJob(TEXT("LoadUIAssets"), LoadUIAssets, 0.3f);
   UIAssetsJob.SubstepProgressDelegate.BindUObject(this, &UMyGameInstance::OnUIAssetsProgress);
   StartupJobs.Add(UIAssetsJob);
}
 进度更新回调函数
void UCancerAssetManager::OnUIAssetsProgress(float Progress)
{
   // 更新UI加载进度
   UE_LOG(LogTemp, Log, TEXT("UI资源加载进度: %.2f%%"), Progress * 100.0f);
}*/


UCancerAssetManager::UCancerAssetManager()
{
}

void UCancerAssetManager::OnAbilityLoaded(const TArray<UObject*>& LoadedAssets)
{
	UE_LOG(CancerAssetManagerLog, Display, TEXT("Loaded Ability: %d"), LoadedAssets.Num());

	for (auto Ability : LoadedAssets)
	{
		LoadedAbilities.AddUnique(Ability->GetClass());
		UE_LOG(CancerAssetManagerLog, Display, TEXT("Loaded Ability: %s"), *Ability->GetName());
	}
}

void UCancerAssetManager::LoadGameplayAbility()
{
	OnAssetsLoaded.AddDynamic(this, &UCancerAssetManager::OnAbilityLoaded);
	const UCancerAssetSetting* AssetSetting = GetMutableDefault<UCancerAssetSetting>();
	Get().LoadAssetsOfClass(UGameplayAbility::StaticClass(),
	                        AssetSetting->ItemTableDataAssetPathArray, OnAssetsLoaded);
}

void UCancerAssetManager::LoadPrimaryAssetsFromSettings()
{
	const UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	FOnAssetsLoaded Dummy;
	if (Settings)
	{
		for (const FPrimaryAssetTypeInfo& TypeInfo : Settings->PrimaryAssetTypesToScan)
		{
			LoadPrimaryAssetsByType(TypeInfo.PrimaryAssetType, /*LoadBundles*/ TArray<FName>(), Dummy);
		}
	}
}

void UCancerAssetManager::LoadPrimaryAssetsByType(const FPrimaryAssetType& AssetType, const TArray<FName>& LoadBundles,
                                                  const FOnAssetsLoaded& OnLoaded)
{
	TArray<FPrimaryAssetId> AssetIds;
	GetPrimaryAssetIdList(AssetType, AssetIds);
	LoadPrimaryAssetsByIds(AssetIds, LoadBundles, OnLoaded);
}

void UCancerAssetManager::LoadPrimaryAssetsByIds(const TArray<FPrimaryAssetId>& AssetIds,
                                                 const TArray<FName>& LoadBundles, const FOnAssetsLoaded& OnLoaded)
{
	if (AssetIds.Num() == 0)
	{
		if (OnLoaded.IsBound())
		{
			OnLoaded.Broadcast(TArray<UObject*>());
		}
		return;
	}

	TArray<FSoftObjectPath> AssetsToLoad;
	for (const FPrimaryAssetId& Id : AssetIds)
	{
		const FSoftObjectPath Path = GetPrimaryAssetPath(Id);
		if (Path.IsValid())
		{
			AssetsToLoad.Add(Path);
		}
	}

	if (AssetsToLoad.Num() == 0)
	{
		if (OnLoaded.IsBound())
		{
			OnLoaded.Broadcast(TArray<UObject*>());
		}
		return;
	}

	TSharedRef<FOnAssetsLoaded> CallbackPtr = MakeShared<FOnAssetsLoaded>(OnLoaded);

	TArray<FSoftObjectPath> PathsCopy = AssetsToLoad;
	FStreamableDelegate OnComplete = FStreamableDelegate::CreateLambda(
		[this, PathsCopy, CallbackPtr]()
		{
			TArray<UObject*> LoadedObjects;
			for (const FSoftObjectPath& AssetPath : PathsCopy)
			{
				if (UObject* LoadedAsset = AssetPath.ResolveObject())
				{
					LoadedObjects.Add(LoadedAsset);
					AddLoadedAsset(LoadedAsset);
				}
			}

			if (CallbackPtr->IsBound())
			{
				CallbackPtr->Broadcast(LoadedObjects);
			}
		}
	);

	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssets(const_cast<TArray<FPrimaryAssetId>&>(AssetIds),
	                                                         LoadBundles, OnComplete);
}

UObject* UCancerAssetManager::LoadPrimaryAssetSync(const FPrimaryAssetId& AssetId, const TArray<FName>& LoadBundles)
{
	const FSoftObjectPath Path = GetPrimaryAssetPath(AssetId);
	if (!Path.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAsset(AssetId, LoadBundles);
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete();
	}

	UObject* LoadedAsset = Path.ResolveObject();
	if (LoadedAsset)
	{
		AddLoadedAsset(LoadedAsset);
	}
	return LoadedAsset;
}

TArray<UObject*> UCancerAssetManager::LoadPrimaryAssetsSync(const TArray<FPrimaryAssetId>& AssetIds,
                                                            const TArray<FName>& LoadBundles)
{
	TArray<UObject*> Result;
	if (AssetIds.Num() == 0)
	{
		return Result;
	}

	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssets(const_cast<TArray<FPrimaryAssetId>&>(AssetIds),
	                                                         LoadBundles);
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete();
	}

	for (const FPrimaryAssetId& Id : AssetIds)
	{
		const FSoftObjectPath Path = GetPrimaryAssetPath(Id);
		if (UObject* LoadedAsset = Path.ResolveObject())
		{
			Result.Add(LoadedAsset);
			AddLoadedAsset(LoadedAsset);
		}
	}
	return Result;
}

TArray<TSubclassOf<UGameplayAbility>> UCancerAssetManager::GetLoadedGameplayAbilities()
{
	return LoadedAbilities;
}

void UCancerAssetManager::LoadAssetsOfClass(TSubclassOf<UObject> AssetClass, const TArray<FDirectoryPath>& AssetPaths,
                                            const FOnAssetsLoaded& OnLoaded) const
{
	if (AssetClass == nullptr)
	{
		UE_LOG(CancerAssetManagerLog, Error, TEXT("AssetClass is null!"));
		return;
	}

	if (AssetPaths.Num() == 0)
	{
		UE_LOG(CancerAssetManagerLog, Warning, TEXT("No asset paths provided!"));
		return;
	}

	// 收集所有路径
	TArray<FString> PathsToScan;
	for (const FDirectoryPath& Path : AssetPaths)
	{
		if (!Path.Path.IsEmpty())
		{
			PathsToScan.Add(Path.Path);
		}
	}

	if (PathsToScan.Num() == 0)
	{
		UE_LOG(CancerAssetManagerLog, Warning, TEXT("No valid paths found!"));
		return;
	}

	// 构建过滤器
	FARFilter Filter;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true; //包含派生类
	Filter.bRecursivePaths = true;
	Filter.TagsAndValues.Add(FBlueprintTags::NativeParentClassPath, FObjectPropertyBase::GetExportPath(AssetClass));
	Filter.TagsAndValues.Add(FBlueprintTags::ParentClassPath, FObjectPropertyBase::GetExportPath(AssetClass));

	for (const FString& Path : PathsToScan)
	{
		Filter.PackagePaths.Add(FName(*Path));
	}
	TArray<FAssetData> AssetDataList;

	// 使用资产注册表查找资产
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetRegistryModule::GetRegistry().GetAssets(Filter, AssetDataList);


	if (AssetDataList.Num() == 0)
	{
		UE_LOG(CancerAssetManagerLog, Warning, TEXT("No assets found of class %s in specified paths"),
		       *AssetClass->GetName());
		if (OnLoaded.IsBound())
		{
			OnLoaded.Broadcast(TArray<UObject*>());
		}
		return;
	}

	// 收集资产路径
	TArray<FSoftObjectPath> AssetsToLoad;
	for (const FAssetData& AssetData : AssetDataList)
	{
		AssetsToLoad.Add(AssetData.ToSoftObjectPath());
	}

	// 异步加载资产
	FStreamableManager& Manager = GetStreamableManager();

	// 创建共享指针来保存回调
	TSharedRef<FOnAssetsLoaded> CallbackPtr = MakeShared<FOnAssetsLoaded>(OnLoaded);
	FStreamableDelegate OnComplete = FStreamableDelegate::CreateLambda(
		[this, AssetsToLoad, CallbackPtr]()
		{
			// 收集所有已加载的资产
			TArray<UObject*> LoadedObjects;
			for (const FSoftObjectPath& AssetPath : AssetsToLoad)
			{
				if (UObject* LoadedAsset = AssetPath.ResolveObject())
				{
					LoadedObjects.Add(LoadedAsset);
				}
			}

			// 调用原始回调
			if (CallbackPtr->IsBound())
			{
				CallbackPtr->Broadcast(LoadedObjects);
			}
		}
	);

	TSharedPtr<FStreamableHandle> Handle = Manager.RequestAsyncLoad(
		AssetsToLoad,
		OnComplete,
		FStreamableManager::AsyncLoadHighPriority);
}

TArray<UObject*> UCancerAssetManager::LoadAssetsOfClassSync(TSubclassOf<UObject> AssetClass,
                                                            const TArray<FDirectoryPath>& AssetPaths)
{
	TArray<UObject*> Result;

	if (AssetClass == nullptr)
	{
		UE_LOG(CancerAssetManagerLog, Error, TEXT("AssetClass is null!"));
		return Result;
	}

	if (AssetPaths.Num() == 0)
	{
		UE_LOG(CancerAssetManagerLog, Warning, TEXT("No asset paths provided!"));
		return Result;
	}

	// 收集所有路径
	TArray<FString> PathsToScan;
	for (const FDirectoryPath& Path : AssetPaths)
	{
		if (!Path.Path.IsEmpty())
		{
			PathsToScan.Add(Path.Path);
		}
	}

	if (PathsToScan.Num() == 0)
	{
		UE_LOG(CancerAssetManagerLog, Warning, TEXT("No valid paths found!"));
		return Result;
	}

	// 使用资产注册表查找资产
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");

	// 构建过滤器
	FARFilter Filter;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true; //包含派生类
	Filter.bRecursivePaths = true;
	Filter.TagsAndValues.Add(FBlueprintTags::NativeParentClassPath, FObjectPropertyBase::GetExportPath(AssetClass));
	Filter.TagsAndValues.Add(FBlueprintTags::ParentClassPath, FObjectPropertyBase::GetExportPath(AssetClass));

	for (const FString& Path : PathsToScan)
	{
		Filter.PackagePaths.Add(FName(*Path));
	}

	// 查找资产
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	if (AssetDataList.Num() == 0)
	{
		UE_LOG(CancerAssetManagerLog, Warning, TEXT("No assets found of class %s in specified paths"),
		       *AssetClass->GetName());
		return Result;
	}

	// 同步加载资产
	for (const FAssetData& AssetData : AssetDataList)
	{
		UObject* LoadedAsset = AssetData.GetAsset();
		if (LoadedAsset)
		{
			Result.Add(LoadedAsset);
		}
	}

	return Result;
}

UCancerAssetManager& UCancerAssetManager::Get()
{
	check(GEngine);
	if (UCancerAssetManager* Singleton = Cast<UCancerAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(CancerAssetManagerLog, Fatal,
	       TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to CancerAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UCancerAssetManager>();
}

void UCancerAssetManager::DumpLoadedAssets()
{
	UE_LOG(CancerAssetManagerLog, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().CacheLoadedAssets)
	{
		UE_LOG(CancerAssetManagerLog, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(CancerAssetManagerLog, Log, TEXT("... %d assets in loaded pool"), Get().CacheLoadedAssets.Num());
	UE_LOG(CancerAssetManagerLog, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

UObject* UCancerAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// 1. 检查资源路径是否有效
	if (AssetPath.IsValid())
	{
		// 2. 记录加载时间（如果启用了资源加载日志）
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(
				*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr,
				FScopeLogTime::ScopeLog_Seconds);
		}

		// 3. 检查资源管理器是否已初始化
		if (UAssetManager::IsInitialized())
		{
			// 4. 使用资源管理器的流式加载器同步加载资源
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// 5. 如果资源管理器未初始化，则直接使用 TryLoad 加载
		return AssetPath.TryLoad();
	}
	// 6. 路径无效，返回空指针
	return nullptr;
}

bool UCancerAssetManager::ShouldLogAssetLoads()
{
	//启用加载日志
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UCancerAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		// 使用作用域锁确保线程安全
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		CacheLoadedAssets.Add(Asset);
	}
}

void UCancerAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UCancerAssetManager::StartInitialLoading");

	Super::StartInitialLoading();


	//STARTUP_JOB_WEIGHTED(LoadGameplayAbility(), 25.0f);
	//STARTUP_JOB_WEIGHTED(LoadPrimaryAssetsFromSettings(), 10.0f);
	// Run all the queued up startup jobs
	//DoAllStartupJobs();
}

#if WITH_EDITOR
void UCancerAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);
	// 创建一个作用域，确保 SlowTask 在作用域结束时自动销毁
	/*{
		// 1. 创建并显示慢任务对话框
		FScopedSlowTask SlowTask(0, NSLOCTEXT("LyraEditor", "BeginLoadingPIEData", "Loading PIE Data"));
		const bool bShowCancelButton = false; // 不显示取消按钮
		const bool bAllowInPIE = true; // 允许在 PIE 中显示
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
		// 2. 预加载游戏数据

		//const ULyraGameData& LocalGameDataCommon = GetGameData();

		// 3. 记录预加载耗时
		// 注意：SCOPE_LOG_TIME_IN_SECONDS 是一个宏，用于测量代码块执行时间
		// 这里特意在 GetGameData() 之后开始计时，不包含游戏数据加载时间
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// 4. 可以在这里添加其他需要在 PIE 开始前预加载的资源
		// 例如：从世界设置获取默认体验，或从开发者设置中获取体验覆盖等
	}*/
}
#endif
void UCancerAssetManager::DoAllStartupJobs()
{
	// 记录启动时间，用于性能分析
	SCOPED_BOOT_TIMING("UCancerAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// 专用服务器不需要进度更新，直接执行所有作业
		for (const FAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		// 客户端或非专用服务器模式，需要处理进度更新
		if (StartupJobs.Num() > 0)
		{
			// 计算所有作业的总权重
			float TotalJobValue = 0.0f;
			for (const FAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}
			// 累计已完成作业的权重
			float AccumulatedJobValue = 0.0f;
			// 遍历并执行每个作业
			for (FAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;

				// 绑定进度更新委托
				StartupJob.SubstepProgressDelegate.BindLambda(
					[This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						// 计算当前作业的进度贡献值
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
						// 计算总体进度百分比
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) /
							TotalJobValue;
						// 更新加载进度
						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				// 执行当前作业
				StartupJob.DoJob();

				// 解绑进度更新委托
				StartupJob.SubstepProgressDelegate.Unbind();
				// 更新累计进度
				AccumulatedJobValue += JobValue;
				// 更新总体进度（确保即使子步骤没有报告进度也能更新）
				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			// 没有启动作业，直接标记为100%完成
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}
	// 清空作业列表
	StartupJobs.Empty();
	// 记录总耗时
	UE_LOG(CancerAssetManagerLog, Display, TEXT("All startup jobs took %.2f seconds to complete"),
	       FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UCancerAssetManager::InitializeGameplayCueManager()
{
}

void UCancerAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// 可以将其路由到早期启动加载屏幕
	UE_LOG(CancerAssetManagerLog, Display, TEXT("A %.2f "), GameContentPercent);
}

UE_ENABLE_OPTIMIZATION
