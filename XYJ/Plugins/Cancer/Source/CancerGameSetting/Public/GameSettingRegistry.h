#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameSetting.h"
#include "GameSettingCollection.h"
#include "GameSettingRegistry.generated.h"

/**
 * 游戏设置注册表
 * 管理所有游戏设置的保存、加载和初始化
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Setting Registry"))
class CANCERGAMESETTING_API UGameSettingRegistry : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGameSettingRegistry();

	/**
	 * 获取单例实例
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	static UGameSettingRegistry* Get();

	// ========== 数据持久化功能 ==========
	/**
	 * 保存设置到配置文件
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SaveSettings();

	/**
	 * 从配置文件加载设置
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void LoadSettings();

	/**
	 * 注册设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void RegisterSetting(UGameSetting* Setting);

	/**
	 * 注销设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void UnregisterSetting(UGameSetting* Setting);

	/**
	 * 根据开发名称查找设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	UGameSetting* FindSettingByDevName(const FName& DevName) const;

	// ========== 新增：设置初始化功能 ==========
	/**
	 * 初始化所有游戏设置（在游戏启动时调用一次）
	 */
	UFUNCTION(BlueprintCallable, Category = "设置管理")
	void InitializeGameSettings();

	/**
	 * 获取设置集合（按分类）
	 */
	UFUNCTION(BlueprintCallable, Category = "设置管理")
	UGameSettingCollection* GetSettingCollection(const FString& InCategoryName) const;

	/**
	 * 获取所有设置集合
	 */
	UFUNCTION(BlueprintCallable, Category = "设置管理")
	TArray<UGameSettingCollection*> GetAllCollections() const;

protected:
	/** 已注册的设置项 */
	UPROPERTY()
	TArray<TObjectPtr<UGameSetting>> RegisteredSettings;

	/** 设置值映射（DevName -> Value） */
	UPROPERTY(Config)
	TMap<FString, FString> SettingValues;

	/** 设置集合映射（CategoryName -> Collection） */
	UPROPERTY()
	TMap<FString, TObjectPtr<UGameSettingCollection>> SettingCollections;

private:
	/** 是否已初始化 */
	bool bIsInitialized = false;

	// ========== 设置创建方法 ==========
	/**
	 * 创建图形设置集合
	 */
	UGameSettingCollection* CreateGraphicsSettings();

	/**
	 * 创建音频设置集合
	 */
	UGameSettingCollection* CreateAudioSettings();

	/**
	 * 创建控制设置集合
	 */
	UGameSettingCollection* CreateControlsSettings();

	/**
	 * 创建游戏性设置集合
	 */
	UGameSettingCollection* CreateGameplaySettings();

	/**
	 * 创建辅助功能设置集合
	 */
	UGameSettingCollection* CreateAccessibilitySettings();

	/**
	 * 创建高级设置集合
	 */
	UGameSettingCollection* CreateAdvancedSettings();
};