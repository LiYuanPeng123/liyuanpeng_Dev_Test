#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerTeamFunctionLibrary.generated.h"

class UCancerTeamComponent;
/**
 * 团队函数库
 */
UCLASS()
class CANCERTEAM_API UCancerTeamFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#pragma  region 获取UTeamManagerComponent
	template <class T = UCancerTeamComponent>
	static T* GetTeamManagerComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	};
	UFUNCTION(BlueprintCallable, Category = CancerTeam, DisplayName="Get TeamManagerComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerTeamComponent* K2_GetTeamManagerComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerTeamComponent> OutputClass = nullptr);
#pragma endregion

#pragma region 查询团队
	UFUNCTION(BlueprintPure, Category = CancerTeam)
	static FGameplayTag GetTeamTagForActor(AActor* Actor);
	UFUNCTION(BlueprintPure, Category = CancerTeam)
	static FGenericTeamId GetGenericTeamIdForActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = CancerTeam)
	static ETeamAttitude::Type GetTeamAttitude(AActor* SelfActor, AActor* Target);
#pragma endregion

#pragma region 变更阵营
	UFUNCTION(BlueprintCallable, Category = CancerTeam)
	static void SetSelfTeamTag(AActor*SelfActor,FGameplayTag TargetTeamTag);
#pragma endregion
};
