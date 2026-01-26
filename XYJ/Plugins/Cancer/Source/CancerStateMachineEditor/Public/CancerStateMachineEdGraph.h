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
};
