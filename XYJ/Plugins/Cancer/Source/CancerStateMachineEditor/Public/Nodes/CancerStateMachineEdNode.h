#pragma once

#include "CoreMinimal.h"
#include "CancerStateMachineEdNodeBase.h"
#include "CancerStateMachineNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "CancerStateMachineEdNode.generated.h"

/**
 * UCancerStateMachineEdNode
 * 编辑器中的状态节点
 * 包装了运行时的 UCancerStateMachineNode
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNode : public UCancerStateMachineEdNodeBase
{
	GENERATED_BODY()

public:
	/** 关联的运行时节点逻辑 */
	UPROPERTY(EditAnywhere, Instanced, Category = "Settings", meta = (DisplayPriority = "1", ShowInnerProperties,
		AllowNativeClasses = "false", ExcludeClasses = "CancerStateMachineNode, CancerStateMachineNode_Task"))
	TObjectPtr<UCancerStateMachineNode> RuntimeNode;

	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual bool GetCanRenameNode() const override { return true; }
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	//~ End UEdGraphNode Interface

	/** 自定义双击处理 */
	virtual void OnNodeDoubleClicked();

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
	//~ End UObject Interface
};

/**
 * UCancerStateMachineEdNode_Task
 * 任务型状态节点 (Type A)
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNode_Task : public UCancerStateMachineEdNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode Interface

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
};

/**
 * UCancerStateMachineEdNode_SubStateMachine
 * 子状态机节点 (HSM)
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNode_SubStateMachine : public UCancerStateMachineEdNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode Interface

	//~ Begin UCancerStateMachineEdNode Interface
	virtual void OnNodeDoubleClicked() override;
	//~ End UCancerStateMachineEdNode Interface
};

/**
 * UCancerStateMachineEdNode_CustomBP
 * 蓝图逻辑型状态节点 (Type B)
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNode_CustomBP : public UCancerStateMachineEdNode
{
	GENERATED_BODY()

public:
	/** 是否是在执行 PerformAction 时请求创建新资产 */
	bool bRequestCreateNewAsset = false;

	//~ Begin UEdGraphNode Interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode Interface
};





/**
 * Inline Blueprint Editor Helper
 * 用于安全地创建 / 修复 / 打开 内嵌蓝图（非独立资产）
 */
class FInlineBlueprintEditorHelper
{
public:
	static void OpenAndFocus(UBlueprint* Blueprint);
private:
	static UEdGraph* EnsureEventGraph(UBlueprint* Blueprint);
};

/**
 * UCancerStateMachineEdNode_Inline
 * 一次性逻辑状态节点 (Type C)
 * 允许用户直接在节点内部编写逻辑，无需预先创建资产
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdNode_Inline : public UCancerStateMachineEdNode
{
	GENERATED_BODY()

public:
	UCancerStateMachineEdNode_Inline();

	/** 内部持有的蓝图对象 (存储在节点内部，不是独立资产) */
	UPROPERTY()
	TObjectPtr<class UCancerStateMachineNodeBlueprint> InnerBlueprint;

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void DestroyNode() override;
	//~ End UEdGraphNode Interface

	//~ Begin UCancerStateMachineEdNode Interface
	virtual void OnNodeDoubleClicked() override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	//~ End UCancerStateMachineEdNode Interface

	/** 初始化内部蓝图 */
	void CreateInnerBlueprint();

protected:
	/** 提升为正式资产 */
	void OnPromoteToAsset();

	/** 确保 RuntimeNode 与内部蓝图同步 */
	void RefreshRuntimeNode();

	/** 监听蓝图编译 */
	void OnInnerBlueprintCompiled(UBlueprint* BP);

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
};
