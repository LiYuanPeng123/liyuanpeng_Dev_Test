#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CancerCoreSystemInterface.generated.h"


class UCancerCombatData;
class USceneComponent;
class UAnimInstance;
class USkeletalMeshComponent;

UINTERFACE()
class UCancerCoreSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CANCERCORE_API ICancerCoreSystemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CancerCore System Interface", DisplayName="获取战斗角色网格")
	USkeletalMeshComponent* GetCombatMesh() const;
	virtual USkeletalMeshComponent* GetCombatMesh_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CancerCore System Interface",
		DisplayName="获取战斗角色动画实例")
	UAnimInstance* GetCombatAnimInstance() const;
	virtual UAnimInstance* GetCombatAnimInstance_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CancerCore System Interface",
		DisplayName="获取战斗角色向前箭头")
	USceneComponent* GetCombatForwardReference() const;
	virtual USceneComponent* GetCombatForwardReference_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CancerCore System Interface",
		DisplayName="关闭碰撞")
	void DisableCharacterCollision();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CancerCore System Interface",
		DisplayName="启用碰撞")
	void EnableCharacterCollision();
	
};
