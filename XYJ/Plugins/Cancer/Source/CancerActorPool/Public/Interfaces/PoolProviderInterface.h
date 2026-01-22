#pragma once
#include "CoreMinimal.h"
#include "PoolProviderInterface.generated.h"


/*
 *对象池组件代理接口
 */

class UCancerActorPoolComponent;

UINTERFACE()
class UPoolProviderInterface : public UInterface
{
	GENERATED_BODY()
};

class CANCERACTORPOOL_API IPoolProviderInterface
{
	GENERATED_BODY()

public:
	/**
	 * Provides the Actor Pool Component available.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = " Pool Provider Interface")
	UCancerActorPoolComponent* GetActorPool() const;
	virtual UCancerActorPoolComponent* GetActorPool_Implementation() const;
};
