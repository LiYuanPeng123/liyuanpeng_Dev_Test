#include "CancerPoolableActor.h"
#include "CancerActorPoolComponent.h"

ACancerPoolableActor::ACancerPoolableActor(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = false;
	bIsMarkedForDeferredDestruction = false;
	PoolableActorState = EPoolableActorState::Inactive;
	TimeToLiveOutsidePool = 3.f;
}

void ACancerPoolableActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TimeToLiveHandle);
	Super::EndPlay(EndPlayReason);
}

bool ACancerPoolableActor::IsManagedByPool_Implementation() const
{
	return PoolManager.IsValid() && PoolManager->IsValidLowLevelFast();
}

void ACancerPoolableActor::ExecutePoolableActorState(EPoolableActorState PreviousActorState)
{
	/*- 新状态为 Inactive 且旧状态是 Reserved 或 Active ：
      - 清除 TimeToLiveOutsidePool 计时器。
      - 调用 OnDeactivation() （隐藏、禁用碰撞、禁用 Tick；若标记延迟销毁则立即销毁）。
      - 新状态为 Reserved ：
      - 启动 TimeToLiveOutsidePool 计时器（避免长时间占位不归还）。
      - 新状态为 Active ：
      - 启动 TimeToLiveOutsidePool 计时器。
      - 调用 OnActivation() （显示、启用碰撞、按设定启用 Tick）。*/
	const bool bWasActiveOrReserved = PreviousActorState == EPoolableActorState::Reserved || PreviousActorState ==
		EPoolableActorState::Active;

	if (Execute_IsInactive(this) && bWasActiveOrReserved)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeToLiveHandle);
		OnDeactivation();
	}
	else if (Execute_IsReserved(this))
	{
		StartTimerToReturnToPool();
	}
	else if (Execute_IsActive(this))
	{
		StartTimerToReturnToPool();
		OnActivation();
	}
}

void ACancerPoolableActor::Reserve_Implementation()
{
	if (!Execute_IsReserved(this) && !Execute_IsActive(this))
	{
		const EPoolableActorState CurrentState = PoolableActorState;
		PoolableActorState = EPoolableActorState::Reserved;
		ExecutePoolableActorState(CurrentState);
	}
}

void ACancerPoolableActor::Activate_Implementation()
{
	if (!Execute_IsActive(this))
	{
		const EPoolableActorState CurrentState = PoolableActorState;
		PoolableActorState = EPoolableActorState::Active;
		ExecutePoolableActorState(CurrentState);
	}
}

void ACancerPoolableActor::Deactivate_Implementation()
{
	//对象池无效 或者 已经销毁时 销毁对象 
	if (!Execute_IsManagedByPool(this) && !bIsMarkedForDeferredDestruction)
	{
		Destroy();
	}
	else if (Execute_IsReserved(this) || Execute_IsActive(this))
	{
		const EPoolableActorState CurrentState = PoolableActorState;
		PoolableActorState = EPoolableActorState::Inactive;
		ExecutePoolableActorState(CurrentState);
	}
}

void ACancerPoolableActor::InitializedByPool_Implementation(UCancerActorPoolComponent* InPool)
{
	PoolManager = TWeakObjectPtr<UCancerActorPoolComponent>(InPool);
}

void ACancerPoolableActor::NotifyPoolDestroyed_Implementation()
{
	if (Execute_IsActive(this))
	{
		bIsMarkedForDeferredDestruction = true;
	}
	else
	{
		Destroy();
	}
}

FGameplayTag ACancerPoolableActor::GetPoolableActorTag_Implementation()
{
	return PoolableActorTag;
}

void ACancerPoolableActor::OnActivation_Implementation()
{
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);

	const bool bShouldTick = PrimaryActorTick.bStartWithTickEnabled;
	SetActorTickEnabled(bShouldTick);
}

void ACancerPoolableActor::OnDeactivation_Implementation()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	// If marked for destruction, we can destroy this now.
	if (bIsMarkedForDeferredDestruction)
	{
		Destroy();
	}
}

void ACancerPoolableActor::HandleTimerExpired()
{
	Execute_Deactivate(this);
}

void ACancerPoolableActor::StartTimerToReturnToPool()
{
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ThisClass::HandleTimerExpired);
	GetWorld()->GetTimerManager().SetTimer(TimeToLiveHandle, Delegate, TimeToLiveOutsidePool, false);
}
