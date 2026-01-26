#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerStateMachineEditorSettings.generated.h"

/**
 * Cancer 状态机编辑器设置
 */
UCLASS(Config = Editor, DefaultConfig, meta = (DisplayName = "Cancer State Machine Editor"))
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCancerStateMachineEditorSettings();

	static UCancerStateMachineEditorSettings* Get();

	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("CancerStateMachineEditor"); }
	//~ End UDeveloperSettings Interface

	/** 入口节点颜色 (绯红) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor EntryNodeColor;

	/** 条件转换节点颜色 (真红色) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor ConditionEdgeColor;

	/** 事件触发节点颜色 (保持不变) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor EventEdgeColor;

	/** 任务节点颜色 (浅蓝) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor TaskNodeColor;

	/** 内联节点颜色 (紫色) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor InlineNodeColor;

	/** 自定义蓝图节点颜色 (深蓝) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor CustomBPNodeColor;

	/** 结束节点颜色 (灰色) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor EndNodeColor;

	/** 默认状态节点颜色 */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor DefaultStateNodeColor;

	/** 全局状态节点颜色 */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor GlobalNodeColor;

	/** 子状态机节点颜色 (青色) */
	UPROPERTY(Config, EditAnywhere, Category = "Node Colors")
	FLinearColor SubStateMachineNodeColor;
};
