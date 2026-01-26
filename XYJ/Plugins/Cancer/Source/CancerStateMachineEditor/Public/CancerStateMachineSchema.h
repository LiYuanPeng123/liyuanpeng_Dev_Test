#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "CancerStateMachineSchema.generated.h"

/** 在图表中添加状态节点的动作 */
USTRUCT()
struct FCancerStateMachineSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TObjectPtr<class UCancerStateMachineEdNode> NodeTemplate;

	FCancerStateMachineSchemaAction_NewNode() : FEdGraphSchemaAction(), NodeTemplate(nullptr) {}
	FCancerStateMachineSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}
	
	/** 执行动作：在指定位置创建新状态节点 */
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
		const FVector2f& Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

/** 在图表中添加全局状态节点的动作 */
USTRUCT()
struct FCancerStateMachineSchemaAction_NewGlobalNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TObjectPtr<class UCancerStateMachineEdNodeGlobal> NodeTemplate;

	FCancerStateMachineSchemaAction_NewGlobalNode() : FEdGraphSchemaAction(), NodeTemplate(nullptr) {}
	FCancerStateMachineSchemaAction_NewGlobalNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}
	
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
		const FVector2f& Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

/** 在图表中添加边（转换）的动作 */
USTRUCT()
struct FCancerStateMachineSchemaAction_NewEdge : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TObjectPtr<class UCancerStateMachineEdNodeEdge> NodeTemplate;

	FCancerStateMachineSchemaAction_NewEdge() : FEdGraphSchemaAction(), NodeTemplate(nullptr) {}
	FCancerStateMachineSchemaAction_NewEdge(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}
	
	/** 执行动作：在指定位置创建新边节点 */
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
		const FVector2f& Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

/** 在图表中添加事件边（转换）的动作 */
USTRUCT()
struct FCancerStateMachineSchemaAction_NewEventEdge : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TObjectPtr<class UCancerStateMachineEdNodeEventEdge> NodeTemplate;

	FCancerStateMachineSchemaAction_NewEventEdge() : FEdGraphSchemaAction(), NodeTemplate(nullptr) {}
	FCancerStateMachineSchemaAction_NewEventEdge(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}
	
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
		const FVector2f& Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

/** 在图表中添加注释的动作 */
USTRUCT()
struct FCancerStateMachineSchemaAction_AddComment : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	FCancerStateMachineSchemaAction_AddComment() : FEdGraphSchemaAction() {}
	FCancerStateMachineSchemaAction_AddComment(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping) {}
	FCancerStateMachineSchemaAction_AddComment(FText InMenuDesc, FText InToolTip)
		: FEdGraphSchemaAction(FText::GetEmpty(), InMenuDesc, InToolTip, 0) {}
	
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
		const FVector2f& Location, bool bSelectNewNode = true) override;
};

/**
 * UCancerStateMachineSchema
 * 状态机图表的 Schema
 * 定义了图表的规则、右键菜单内容以及节点连接逻辑
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	// 预定义一些颜色
	static const FName PC_Edge;
	static const FLinearColor Color_StateNode;
	static const FLinearColor Color_EdgeNode;
	static const FLinearColor Color_EntryNode;
	static const FLinearColor Color_GlobalNode;

	//~ Begin UEdGraphSchema Interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
	//~ End UEdGraphSchema Interface
};
