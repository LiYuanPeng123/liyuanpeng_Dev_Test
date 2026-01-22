#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "AnimationAssetAction.generated.h"

class UAnimationModifier;
/**
 *  动画资产编辑
 */
UCLASS()
class CANCERANIMATIONEDITOR_API UAnimationAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, DisplayName="移除所有曲线和通知")
	void EditAnimation_RemoveCurvesAndNotifies();

	UFUNCTION(CallInEditor, DisplayName="移除所有曲线")
	void EditAnimation_Curves();

	UFUNCTION(CallInEditor, DisplayName="移除所有通知")
	void EditAnimation_Notifies();

	UFUNCTION(CallInEditor, DisplayName="添加动画通知")
	void EditAnimation_AddNotifies(TMap<FName, TSubclassOf<UAnimNotify>> NotifyMaps);
	UFUNCTION(CallInEditor, DisplayName="添加动画通知状态")
	void EditAnimation_AddNotifyState(TMap<FName, TSubclassOf<UAnimNotifyState>> NotifyStateMaps);

	UFUNCTION(CallInEditor, DisplayName="添加动画修改器")
	void EditAnimation_AddModifier(TArray<TSubclassOf<UAnimationModifier>> Modifier);

	UFUNCTION(CallInEditor, DisplayName="设置动画曲线压缩设置")
	void EditAnimation_SetCurveSetting(UAnimCurveCompressionSettings* Obj);

	UFUNCTION(CallInEditor, DisplayName="导出fpx")
	void ExportSelectedToFBX();
};
