#pragma once
#include "CoreMinimal.h"
#include "FPoolableActorInfo.generated.h"


USTRUCT(BlueprintType)
struct FPoolableActorInfo
{
	GENERATED_BODY()

	/** Class that will be pooled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poolable Actor Info",
		Meta = (MustImplement = "/Script/CancerActorPool.PoolableActorInterface"))
	TSubclassOf<AActor> PoolableActorClass = nullptr;

	/** Size of this pool. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poolable Actor Info")
	uint8 PoolSize = 0;
};
