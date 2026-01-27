#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "CancerStateMachineData.h"
#include "CancerStateMachineEdGraph.generated.h"

/**
 * UCancerStateMachineEdGraph
 * 状态机编辑器使用的图表类
 * 负责可视化节点与运行时数据之间的同步
 */
UCLASS()
class CANCERSTATEMACHINEEDITOR_API UCancerStateMachineEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	/** 关联的状态机数据资产 */
	UPROPERTY()
	TObjectPtr<UCancerStateMachineData> StateMachineData;

	/**
	 * 从运行时数据重建可视化图表
	 */
	void RebuildGraph();

	/**
	 * 将可视化图表保存到运行时数据
	 */
	void SaveGraph();

private:
	/** 同步所有节点并返回 EdNode -> RuntimeNode 的映射 */
	void SyncNodes(TMap<UEdGraphNode*, UObject*>& OutEdToRuntimeMap);

	/** 构建所有连接关系 (包含 Edge 的实例化和属性同步) */
	void BuildConnections(const TMap<UEdGraphNode*, UObject*>& EdToRuntimeMap);

	/** 辅助函数：处理单个输出引脚的连接 */
	void ProcessOutputPinConnections(UEdGraphPin* OutPin, class UCancerStateMachineNode* StartRuntimeNode, const TMap<UEdGraphNode*, UObject*>& EdToRuntimeMap);
};
