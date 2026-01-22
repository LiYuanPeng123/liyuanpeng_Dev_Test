// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AssetPrefixSuffixAction.generated.h"

/**
 * 资产前缀/后缀管理工具类
 * 用于批量为资产添加或删除前缀/后缀
 */
UCLASS()
class CANCERASSETEDITOR_API UAssetPrefixSuffixAction : public UAssetActionUtility
{
	GENERATED_BODY()

	
public:
	/**
	 * 打开前缀/后缀编辑窗口
	 * 提供图形界面来编辑资产的前缀和后缀
	 */
	UFUNCTION(CallInEditor)
	void OpenPrefixSuffixWindow();

	/**
	 * 为选中的资产添加前缀
	 * @param Prefix - 要添加的前缀
	 * @param bSkipExisting - 是否跳过已有该前缀的资产
	 */
	UFUNCTION()
	void AddPrefixToSelectedAssets(const FString& Prefix, bool bSkipExisting = true);

	/**
	 * 为选中的资产添加后缀
	 * @param Suffix - 要添加的后缀
	 * @param bSkipExisting - 是否跳过已有该后缀的资产
	 */
	UFUNCTION()
	void AddSuffixToSelectedAssets(const FString& Suffix, bool bSkipExisting = true);

	/**
	 * 从选中的资产中移除前缀
	 * @param Prefix - 要移除的前缀
	 * @param bOnlyExact - 是否只移除完全匹配的前缀
	 */
	UFUNCTION()
	void RemovePrefixFromSelectedAssets(const FString& Prefix, bool bOnlyExact = true);

	/**
	 * 从选中的资产中移除后缀
	 * @param Suffix - 要移除的后缀
	 * @param bOnlyExact - 是否只移除完全匹配的后缀
	 */
	UFUNCTION()
	void RemoveSuffixFromSelectedAssets(const FString& Suffix, bool bOnlyExact = true);

private:
	/** 预览资产名称修改 */
	void PreviewChanges(const TArray<FAssetData>& Assets, const FString& Prefix, const FString& Suffix, bool bIsAdding);

	/**
	 * 检查资产名称是否包含指定前缀
	 * @param AssetName - 资产名称
	 * @param Prefix - 要检查的前缀
	 * @param bExactMatch - 是否要求完全匹配
	 * @return bool - 是否包含指定前缀
	 */
	bool HasPrefix(const FString& AssetName, const FString& Prefix, bool bExactMatch = true) const;

	/**
	 * 检查资产名称是否包含指定后缀
	 * @param AssetName - 资产名称
	 * @param Suffix - 要检查的后缀
	 * @param bExactMatch - 是否要求完全匹配
	 * @return bool - 是否包含指定后缀
	 */
	bool HasSuffix(const FString& AssetName, const FString& Suffix, bool bExactMatch = true) const;

	/**
	 * 重命名资产
	 * @param Asset - 要重命名的资产
	 * @param NewName - 新的资产名称
	 * @return bool - 重命名是否成功
	 */
	bool RenameAsset(const FAssetData& Asset, const FString& NewName) const;

	/** 主编辑窗口引用 */
	TSharedPtr<SWindow> MainEditorWindow;
};
