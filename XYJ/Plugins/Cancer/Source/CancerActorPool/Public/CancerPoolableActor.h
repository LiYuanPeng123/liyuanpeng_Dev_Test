#pragma once
#include "CoreMinimal.h"

#include "Interfaces/PoolableActorInterface.h"
#include "Types/EPoolableActorState.h"
#include "CancerPoolableActor.generated.h"


class UCancerActorPoolComponent;

/*
 * 可池化Actor基类：实现对象池接口与状态切换
 */
UCLASS(Abstract)
class CANCERACTORPOOL_API ACancerPoolableActor : public AActor, public IPoolableActorInterface
{
	GENERATED_BODY()

public:
	ACancerPoolableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// -- Begin Actor implementation
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// -- End Actor implementation

	// -- Begin Poolable Actor Implementation
	virtual bool IsManagedByPool_Implementation() const override;

	virtual bool IsInactive_Implementation() const override
	{
		return PoolableActorState == EPoolableActorState::Inactive;
	}

	virtual bool IsReserved_Implementation() const final override
	{
		return PoolableActorState == EPoolableActorState::Reserved;
	}

	virtual bool IsActive_Implementation() const final override
	{
		return PoolableActorState == EPoolableActorState::Active;
	}

	UFUNCTION()
	virtual void ExecutePoolableActorState(EPoolableActorState PreviousActorState);

	virtual void Reserve_Implementation() override;
	virtual void Activate_Implementation() override;
	virtual void Deactivate_Implementation() override;
	virtual void InitializedByPool_Implementation(UCancerActorPoolComponent* InPool) override;
	virtual void NotifyPoolDestroyed_Implementation() override;
	virtual FGameplayTag GetPoolableActorTag_Implementation() override;
	// -- End Poolable Actor Implementation

protected:
	// 在池外存在的最大时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Poolable Actor",DisplayName="存活时长")
	float TimeToLiveOutsidePool = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Poolable Actor",DisplayName="唯一标签")
	FGameplayTag PoolableActorTag;

	// 激活时回调：可自定义启用逻辑
	UFUNCTION(BlueprintNativeEvent, Category = "Poolable Actor")
	void OnActivation();
	virtual void OnActivation_Implementation();

	// 失活时回调：可自定义复位/清理逻辑
	UFUNCTION(BlueprintNativeEvent, Category = "Poolable Actor")
	void OnDeactivation();
	virtual void OnDeactivation_Implementation();

	// 池外生存时间到期处理（默认进行Deactivate）
	UFUNCTION()
	virtual void HandleTimerExpired();

	// 启动池外生存计时器（按TimeToLiveOutsidePool设置）
	virtual void StartTimerToReturnToPool();

private:
	/** 管理该Actor的对象池组件 */
	TWeakObjectPtr<UCancerActorPoolComponent> PoolManager;

	/** 该Actor在对象池中的状态 */
	UPROPERTY()
	EPoolableActorState PoolableActorState;

	/** 当对象池已销毁但Actor仍激活时标记，便于延迟销毁 */
	UPROPERTY()
	uint8 bIsMarkedForDeferredDestruction : 1;

	/** 池外生存计时器句柄 */
	FTimerHandle TimeToLiveHandle;
};
