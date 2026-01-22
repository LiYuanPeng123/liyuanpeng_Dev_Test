#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerAIFunctionLibrary.generated.h"

class UAIPatrolComponent;
/**
 *  AI相关函数库
 */
UCLASS()
class CANCERAI_API UCancerAIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 获取AIPatrolComponent（模板版），若Actor无该组件则返回nullptr
	template <class T = UAIPatrolComponent>
	static T* GetAIPatrolComponent(AActor* Actor)
	{
		T* AIPatrolComponent = nullptr;
		if (IsValid(Actor))
		{
			AIPatrolComponent = Actor->FindComponentByClass<T>();
		}
		return AIPatrolComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "AIPatrol", DisplayName="Get AIPatrol Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UAIPatrolComponent* K2_GetAIPatrolComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UAIPatrolComponent> OutputClass = nullptr);

	// 停止AI行为树
	UFUNCTION(BlueprintCallable, Category = "Cancer|AI|BehaviorTree", DisplayName="Stop AI Behavior Tree")
	static bool StopAIBehaviorTree(AActor* AIControlledActor);

	// 开始AI行为树
	UFUNCTION(BlueprintCallable, Category = "Cancer|AI|BehaviorTree", DisplayName="Start AI Behavior Tree")
	static bool StartAIBehaviorTree(AActor* AIControlledActor);
	
	// 寻找武器
	UFUNCTION(BlueprintCallable, Category = "Cancer|AI|BehaviorTree")
	static AActor* FindWeaponActor(APawn* ControlledPawn , FGameplayTag WeaponTag, float MaxDistance, bool ShowDebugRange = false);
};
