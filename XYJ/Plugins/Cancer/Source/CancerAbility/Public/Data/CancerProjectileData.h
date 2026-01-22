
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/FCancerProjectileType.h"
#include "CancerProjectileData.generated.h"

/**
 *  发射物参数
 */
UCLASS()
class CANCERABILITY_API UCancerProjectileData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Projectile")
	FCancerProjectileType ProjectileType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Projectile")
	FCancerProjectileEffect ProjectileEffect;
};
