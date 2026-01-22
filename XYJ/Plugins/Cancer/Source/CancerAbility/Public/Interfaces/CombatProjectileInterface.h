#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatProjectileInterface.generated.h"

class UCancerMeleeScan;
class UCancerProjectileData;
// This class does not need to be modified.
UINTERFACE()
class UCombatProjectileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CANCERABILITY_API ICombatProjectileInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	float GetSpeed() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	void SetTargetLocation(FVector InTargetLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	FVector GetTargetLocation();

	//注册伤害检测实例
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category = "Projectile Interface")
	int32 RegisterMeleeScanRequest(UCancerMeleeScan* MeleeScan);
	
	//取消伤害检测
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category = "Projectile Interface")
	void CancelMeleeScanRequest(int32 RequestId);

	//初始化
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	void InitProjectileData(const UCancerProjectileData* InProjectileData);

	//在弹丸发射前有时间准备任何初始数据。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	void PrepareLaunch();

	// 利用其内部设置发射弹丸。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	void Launch();

	//获取击中目标
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Interface")
	TArray<AActor*> GetActorsHit() const;

	//获取发射物Mesh
	UFUNCTION(BlueprintNativeEvent)
	UMeshComponent* GetProjectileMesh();
	virtual UMeshComponent* GetProjectileMesh_Implementation() { return nullptr; };
};
