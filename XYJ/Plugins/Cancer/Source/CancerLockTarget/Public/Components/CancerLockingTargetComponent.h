#pragma once

// 组件：UCancerLockingTargetComponent
// 用途：为“可被锁定/可发起锁定”的角色（Actor）提供目标锁定的运行时能力。
// 常见绑定对象：角色Pawn/Character上（也可用于可被锁定的敌人/物件）。
//
// 使用指引（快速上手）：
// 1) 添加组件：在角色蓝图或C++中添加 UCancerLockingTargetComponent。
// 2) 设置UI：在 Project Settings -> Cancer -> CancerLockTarget 中配置默认锁定控件，
//    或在本组件上设置 LockingWidgetComponentClass 与 LockingPointWidgetClass。
// 3) 查找并设置锁定点：
//    - 调用 FindLockingPointArray() 获取当前可用的锁定点列表（FFindLockingPointInfo）。
//    - 使用 FindLockingPointByAngle()/FindLockingPointByDistance()/FindLockingPointByX() 等策略挑选目标。
//    - 调用 SetLockingPointComponent() 开始锁定；调用 FinishLockingPointComponent() 结束锁定。
// 4) 方向切换：调用 FindLockingPointByDirection(ChangeDirection) 从当前锁定点沿方向挑选下一个锁定点。
// 5) 事件：
//    - OnLockingPointChangeDelegate：当前锁定点改变（通知自身）。
//    - OnLockingByOtherDelegate：被其它角色锁定。
//    - OnRemoveLockingByOtherDelegate：被其它角色解除锁定。
// 6) 可见性与通畅判断：
//    - IsInScreenViewport(WorldPos) 判断点是否在屏幕视野范围内。
//    - IsNoObstruction(LockingPoint) 判断锁定线是否被障碍物阻挡（需结合设置中的通道/对象类型）。
// 7) 能否锁定：
//    - IsCanLock(LockingPoint) 提供可覆盖逻辑（BlueprintNativeEvent），可按距离/角度/阵营等自定义规则。
//
// 拓展：通过重写 SortLockingPointArray/SortChangeLockingPointArray 实现自定义排序规则，
//       结合 RankLockingPointByDelegate() 对候选列表排序。

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CancerLockingTargetComponent.generated.h"

class UWidgetComponent;
struct FFindLockingPointInfo;
class UCancerLockingPointComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLockingDelegate, UCancerLockingPointComponent*, LockingPointComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLockingSelfDelegate, UCancerLockingPointComponent*,
                                             LockingTargetComponent,
                                             UCancerLockingPointComponent*, LockingPointComponent);

UCLASS(ClassGroup=(Cancer), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERLOCKTARGET_API UCancerLockingTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerLockingTargetComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 最大可锁定距离（单位：厘米）。超过该距离的锁定点将被过滤。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="最大锁定距离")
	float MaxLockingDistance = 3000;

	// 锁定目标更改（自身锁定目标发生变化时触发）
	UPROPERTY(BlueprintAssignable)
	FLockingDelegate OnLockingPointChangeDelegate;
	// 被别人锁定（当其他角色把你作为锁定目标时触发）
	UPROPERTY(BlueprintAssignable)
	FLockingSelfDelegate OnLockingByOtherDelegate;
	// 被别人解除锁定（当其他角色解除对你的锁定时触发）
	UPROPERTY(BlueprintAssignable)
	FLockingSelfDelegate OnRemoveLockingByOtherDelegate;

	// 锁定指示 UI 的组件类型（例如 UWidgetComponent），用于在3D世界中显示锁定标记。
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UWidgetComponent> LockingWidgetComponentClass;

	//拥有任意标签的角色 会被过滤
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer LockContainer;

	// 当前锁定的目标点组件（若为空则表示未锁定）。
	UPROPERTY(BlueprintReadOnly)
	UCancerLockingPointComponent* CurLockingPointComponent;

	// 锁定点对应的 UUserWidget 类（UI 资源），用于显示在目标上方/附近。
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UUserWidget> LockingPointWidgetClass;

	// 用于承载/显示锁定 UI 的实际组件实例。
	UPROPERTY(BlueprintReadOnly)
	UWidgetComponent* LockingPointWidgetComponent;

	// 当前有哪些其他 UCancerLockingTargetComponent 正在锁定“我”。
	UPROPERTY(BlueprintReadOnly)
	TSet<UCancerLockingTargetComponent*> LockingSelfArray;

	//获取锁定目标
	UFUNCTION(BlueprintPure)
	AActor* GetLockTarget() const;
	
	//取消被锁定
	UFUNCTION(BlueprintCallable)
	void PassiveCancellationLock();

	// 将某个组件标记为正在锁定“我”（由对方调用）。
	UFUNCTION()
	void AddLockingSelfComponent(UCancerLockingTargetComponent* LockingComponent,
	                             UCancerLockingPointComponent* LockingPointComponent);
	// 将某个组件从“正在锁定我”的集合中移除（由对方调用）。
	UFUNCTION()
	void RemoveLockingSelfComponent(UCancerLockingTargetComponent* LockingComponent,
	                                UCancerLockingPointComponent* LockingPointComponent);

	// 设置当前锁定点（开始锁定）。可在蓝图中覆盖实现额外逻辑（如播放音效/动画）。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetLockingPointComponent(UCancerLockingPointComponent* LockingPointComponent);
	// 结束当前锁定（清理UI/状态与事件广播）。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FinishLockingPointComponent();
	// 当前是否已存在锁定目标。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsLockTarget();

	// 获取场景中符合条件的锁定点候选列表（会考虑距离/可见性/阻挡等规则）。
	UFUNCTION(BlueprintCallable)
	TArray<FFindLockingPointInfo> FindLockingPointArray();

	// 从候选中根据与视线/前向的夹角选择更优的锁定点。
	UFUNCTION(BlueprintCallable)
	UCancerLockingPointComponent* FindLockingPointByAngle(TArray<FFindLockingPointInfo> LockingPointDataArray);
	// 从候选中按屏幕X轴（左右方向）筛选/排序锁定点，便于左右切换。
	UFUNCTION(BlueprintCallable)
	TArray<FFindLockingPointInfo> FindLockingPointByX(TArray<FFindLockingPointInfo> LockingPointDataArray);
	// 从候选中选择距离最近的锁定点。
	UFUNCTION(BlueprintCallable)
	UCancerLockingPointComponent* FindLockingPointByDistance(TArray<FFindLockingPointInfo> LockingPointDataArray);

	// 以“当前锁定点”为起点，按照给定屏幕方向（ChangeDirection）查找下一个合适锁定点（用于方向切换）。
	UFUNCTION(BlueprintCallable)
	TArray<FFindLockingPointInfo> FindLockingPointByDirection(FVector2D ChangeDirection);

	// 根据可覆盖的排序委托对候选数组进行排序。
	UFUNCTION(BlueprintCallable)
	TArray<FFindLockingPointInfo> RankLockingPointByDelegate(
		const TArray<FFindLockingPointInfo>& LockingPointDataArray);
	// 默认候选排序函数（可在蓝图中覆盖以自定义优先级）。
	UFUNCTION(BlueprintNativeEvent)
	bool SortLockingPointArray(const FFindLockingPointInfo& A, const FFindLockingPointInfo& B);
	// 方向切换时的候选排序函数（可在蓝图中覆盖）。
	UFUNCTION(BlueprintNativeEvent)
	bool SortChangeLockingPointArray(const FFindLockingPointInfo& A, const FFindLockingPointInfo& B);

	// 额外筛选/修正逻辑：从候选中进行最终确认/替换（例如过滤越界、不可见、阵营不符等）。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FFindLockingPointInfo LockingExtraCheck(const TArray<FFindLockingPointInfo>& LockingPointDataArray);

	// 世界坐标点是否在屏幕可视区域内（常用于可见性筛选）。
	UFUNCTION(BlueprintCallable)
	bool IsInScreenViewport(const FVector& WorldPosition);

	// 锁定射线是否未被障碍物阻挡（结合设置中的 Trace 通道与对象类型）。
	UFUNCTION(BlueprintCallable)
	bool IsNoObstruction(UCancerLockingPointComponent* LockingPointComponent);

	// 是否允许锁定该点（可覆盖：距离、角度、层级、阵营等判定）。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsCanLock(UCancerLockingPointComponent* LockingPointComponent);
};
