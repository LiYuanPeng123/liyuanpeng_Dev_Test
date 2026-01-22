
#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AssetDescriptionAction.generated.h"
/**
 * 资产描述编辑工具类
 * 这是一个编辑器工具类，用于在内容浏览器中提供编辑资产描述的功能
 * 继承自UAssetActionUtility，可以在编辑器中作为资产操作工具使用
 * 
 * 使用方法：
 * 1. 在内容浏览器中选择一个或多个资产
 * 2. 右键点击，在上下文菜单中选择"编辑描述"或"逐个编辑描述"
 * 3. 在弹出的窗口中编辑描述
 * 4. 点击确认保存更改，或点击取消放弃更改
 */
UCLASS()
class CANCERASSETEDITOR_API UAssetDescriptionAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, DisplayName="资产描述(批量)")
	void EditSelectedAssetsDescription();

	UFUNCTION(CallInEditor, DisplayName="资产描述(序列)")
	void EditSelectedAssetsDescriptionSequentially();
}; 