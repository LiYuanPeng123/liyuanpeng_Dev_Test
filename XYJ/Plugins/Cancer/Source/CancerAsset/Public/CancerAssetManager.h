#pragma once

#include "CoreMinimal.h"
#include "AssetManagerStartupJob.h"
#include "Engine/AssetManager.h"
#include "CancerAssetManager.generated.h"

class UGameplayAbility;
DECLARE_LOG_CATEGORY_EXTERN(CancerAssetManagerLog, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetsLoaded, const TArray<UObject*>&, LoadedAssets);


/*
 * 资产管理器类
 * 需要在DefaultEngine.ini中配置为默认的AssetManager：
 * [/Script/Engine.Engine]
 * AssetManagerClassName=/Script/CancerAsset.CancerAssetManager
 */
UCLASS()
class CANCERASSET_API UCancerAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UCancerAssetManager();

	UPROPERTY()
	FOnAssetsLoaded OnAssetsLoaded;
	
	UFUNCTION()
	void OnAbilityLoaded(const TArray<UObject*>& LoadedAssets);

	void LoadGameplayAbility();

	void LoadPrimaryAssetsFromSettings();

	void LoadPrimaryAssetsByType(const FPrimaryAssetType& AssetType, const TArray<FName>& LoadBundles, const FOnAssetsLoaded& OnLoaded);

	void LoadPrimaryAssetsByIds(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles, const FOnAssetsLoaded& OnLoaded);

	UObject* LoadPrimaryAssetSync(const FPrimaryAssetId& AssetId, const TArray<FName>& LoadBundles);

	TArray<UObject*> LoadPrimaryAssetsSync(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles);

	
	TArray<TSubclassOf<UGameplayAbility>> GetLoadedGameplayAbilities();

	
	void LoadAssetsOfClass(TSubclassOf<UObject> AssetClass, const TArray<FDirectoryPath>& AssetPaths,
	                       const FOnAssetsLoaded& OnLoaded) const;

	
	TArray<UObject*> LoadAssetsOfClassSync(TSubclassOf<UObject> AssetClass, const TArray<FDirectoryPath>& AssetPaths);


	static UCancerAssetManager& Get();


	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);


	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);



	static void DumpLoadedAssets();

protected:
	
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	static bool ShouldLogAssetLoads();

	void AddLoadedAsset(const UObject* Asset);

	//~UAssetManager 接口重写
	virtual void StartInitialLoading() override;
#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~End of UAssetManager 接口


private:
	/**
   * 执行所有启动任务
   * 处理所有启动时需要完成的工作
   */
	void DoAllStartupJobs();


	void InitializeGameplayCueManager();


	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	// 启动时需要执行的任务列表
	TArray<FAssetManagerStartupJob> StartupJobs;

private:
	// 资产管理器加载和跟踪的资产。(已经加载的资产)
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> CacheLoadedAssets;
	
	UPROPERTY()
	TArray<TSubclassOf<UGameplayAbility>> LoadedAbilities;
	
	FCriticalSection LoadedAssetsCritical;
};


template <typename AssetType>
AssetType* UCancerAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	// 获取资产的软引用路径
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		// 尝试获取已加载的资产
		LoadedAsset = AssetPointer.Get();
		// 如果资产未加载，则同步加载
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			// 确保加载成功，失败时记录错误
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
		// 如果需要保持在内存中，则添加到已加载资产列表
		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UCancerAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer,
                                                        bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;
	// 获取资产类的软引用路径
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	// 检查路径是否有效
	if (AssetPath.IsValid())
	{
		// 尝试获取已加载的资产类
		LoadedSubclass = AssetPointer.Get();
		// 如果资产类未加载，则同步加载
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			// 确保加载成功，失败时记录错误
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}
		// 如果需要保持在内存中，则添加到已加载资产列表
		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
