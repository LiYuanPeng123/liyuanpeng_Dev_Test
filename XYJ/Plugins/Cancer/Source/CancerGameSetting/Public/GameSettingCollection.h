
#pragma once

#include "CoreMinimal.h"
#include "GameSetting.h"
#include "GameSettingCollection.generated.h"

/**
 * 设置项集合管理
 */
UCLASS(BlueprintType, Blueprintable, meta = (Category = "Game Settings"))
class CANCERGAMESETTING_API UGameSettingCollection : public UGameSetting
{
	GENERATED_BODY()

public:
	UGameSettingCollection();

	// 重写基类方法
	virtual TArray<UGameSetting*> GetChildSettings() override { return Settings; }

	/**
	 * 获取子集合
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	TArray<UGameSettingCollection*> GetChildCollections() const;

	/**
	 * 添加设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void AddSetting(UGameSetting* Setting);

	/**
	 * 移除设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void RemoveSetting(UGameSetting* Setting);

	/**
	 * 根据过滤器获取设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	TArray<UGameSetting*> GetSettingsForFilter(const FGameSettingFilterState& FilterState) const;

	/**
	 * 清空所有设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void ClearSettings();

	/**
	 * 获取设置项数量
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	int32 GetSettingCount() const { return Settings.Num(); }

	/**
	 * 根据开发名称查找设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	UGameSetting* FindSettingByDevName(const FName& InDevName) const;

	/**
	 * 检查是否可选择
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	bool IsSelectable() const { return bIsSelectable; }

	/**
	 * 设置是否可选择
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetSelectable(bool bInSelectable) { bIsSelectable = bInSelectable; }

protected:
	/** 设置项列表 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	TArray<TObjectPtr<UGameSetting>> Settings;

	/** 是否可选择 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	bool bIsSelectable = true;
};
