#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PoolableActorInterface.generated.h"


/*
 *对象池对象接口
 */

UINTERFACE()
class UPoolableActorInterface : public UInterface
{
	GENERATED_BODY()
};

class CANCERACTORPOOL_API IPoolableActorInterface
{
	GENERATED_BODY()

public:
	/**
	 * Notifies that this actor was initialized by an Actor Pool.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	void InitializedByPool(UCancerActorPoolComponent* InPool);

	/**
	 * Informs if this Cast Actor is managed by an Actor Pool.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	bool IsManagedByPool() const;

	/**
	 * Checks if this actor is currently inactive.
	 * 
	 * This can also mean that the actor is just not managed by a pool, which means it will never
	 * be "reserved" or "active", even though it can still be fully functional.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	bool IsInactive() const;

	/**
	 * Checks if this actor is currently reserved.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	bool IsReserved() const;

	/**
	 * Checks if the actor is currently active.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	bool IsActive() const;

	/**
	 * Reserves the actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	void Reserve();

	/**
	 * Activates the actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	void Activate();

	/**
	 * Deactivates the actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	void Deactivate();

	/**
	 * Notifies that the owning pool has been destroyed.
	 * This should be taken into consideration when the actor deactivates.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	void NotifyPoolDestroyed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Poolable Actor Interface")
	FGameplayTag GetPoolableActorTag();
};
