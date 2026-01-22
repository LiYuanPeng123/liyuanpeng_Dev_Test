#pragma once

#include "CoreMinimal.h"
#include "Components/CancerLockingPointComponent.h"
#include "Components/CancerLockingTargetComponent.h"
#include "Interfaces/LockingTargetInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerLockingFunctionLibrary.generated.h"


UCLASS()
class CANCERLOCKTARGET_API UCancerLockingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#pragma  region //获取CancerLockingTargetComponent
	template <class T = UCancerLockingTargetComponent>
	static T* GetLockingTargetComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		if (Actor->GetClass()->ImplementsInterface(ULockingTargetInterface::StaticClass()))
		{
			UCancerLockingTargetComponent* LockingComponent =
				ILockingTargetInterface::Execute_GetLockingTargetComponent(Actor);
			return Cast<T>(LockingComponent);
		}
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Locking", DisplayName="Get CancerLockingTargetComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerLockingTargetComponent* K2_GetLockingTargetComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerLockingTargetComponent> OutputClass = nullptr);
#pragma endregion

#pragma region //获取当前锁定组件 锁定目标的锁定点组件
	template <class T = UCancerLockingPointComponent>
	static T* GetLockTarget_PointComponent(AActor* Actor)
	{
		if (UCancerLockingTargetComponent* LockingComponent = GetLockingTargetComponent(Actor))
		{
			return Cast<T>(LockingComponent->CurLockingPointComponent);
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Locking", DisplayName="Get CancerLockingTargetPointComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerLockingPointComponent* K2_GetLockTarget_PointComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerLockingPointComponent> OutputClass = nullptr);
#pragma endregion

#pragma region//传入自己获取锁定目标_Actor
	template <class T = AActor>
	static T* GetLockingTarget_Actor(AActor* Actor)
	{
		if (UCancerLockingPointComponent* LockingTarget = GetLockTarget_PointComponent(Actor))
		{
			return Cast<T>(LockingTarget->GetOwner());
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Locking", DisplayName="Get Locking Target Actor",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class AActor* K2_GetLockingTarget_Actor(
		AActor* Actor, bool& bIsValid, TSubclassOf<AActor> OutputClass = nullptr);
#pragma endregion


#pragma region//获取默认锁定点组件
	template <class T = UCancerLockingPointComponent>
	static class UCancerLockingPointComponent* GetDefaultPointComponent(AActor* Actor)
	{
		if (Actor)
		{
			return Actor->FindComponentByClass<T>();
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Locking", DisplayName="Get Default Point Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerLockingPointComponent* K2_GetDefaultPointComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerLockingPointComponent> OutputClass = nullptr);
#pragma endregion


	//设置锁定目标_锁定点组件
	UFUNCTION(BlueprintCallable, Category = "Locking")
	static void SetLockingTarget_PointComponent(AActor* Actor, UCancerLockingPointComponent* LockingTarget);

	//设置锁定目标_Actor
	UFUNCTION(BlueprintCallable, Category = "Locking")
	static void SetLockingTarget_Actor(AActor* Actor, AActor* LockingTarget);

	//锁定目标阻挡物检测
	UFUNCTION(BlueprintCallable, Category = "Locking")
	static bool TraceBlockBetweenCollision(AActor* StartActor, AActor* EndActor,
	                                       FVector2D DefaultStartSize = FVector2D(50.f, 50.f),
	                                       FVector2D DefaultEndSize = FVector2D(50.f, 50.f),
	                                       float Precision = 0.f);

	//锁定目标阻挡物检测
	UFUNCTION(BlueprintCallable, Category = "Locking")
	static bool TraceBlockByDirectionDistance(AActor* StartActor, FRotator TraceDirection, float TraceDistance,
	                                          FVector TraceSize);

	/**
	* 检查目标是否在扇形区域内
	* @param StartActor 起始角色
	* @param TargetActor 目标角色
	* @param OffsetAngle 偏移角度（度）
	* @param CheckAngle 检测角度范围（度）
	* @param CheckRadius 检测半径
	* @param CheckHeight 检测高度
	* @return 如果目标在扇形区域内返回true，否则返回false
	*/
	UFUNCTION(BlueprintCallable)
	static bool SectorTargetCheck(AActor* StartActor, AActor* TargetActor, float OffsetAngle, float CheckAngle,
	                              float CheckRadius = -1.f, float CheckHeight = -1.f);

	UFUNCTION(BlueprintCallable)
	static bool IsCanLock(AActor* OwnerActor, AActor* TargetActor);
	
};
