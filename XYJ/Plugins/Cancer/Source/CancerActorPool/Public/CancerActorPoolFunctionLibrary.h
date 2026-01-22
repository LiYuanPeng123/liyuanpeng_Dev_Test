#pragma once
#include "CoreMinimal.h"
#include "CancerActorPoolFunctionLibrary.generated.h"


class UCancerActorPoolComponent;
/*
 *对象池相关函数库
 */
UCLASS()
class CANCERACTORPOOL_API UCancerActorPoolFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename T = UCancerActorPoolComponent>
	static T* GetCancerActorPoolComponent(const AActor* Actor)
	{
		if (Actor == nullptr) return nullptr;
		return Actor->GetComponentByClass<T>();
	}
	UFUNCTION(BlueprintCallable,Category= ActorPool,meta=(DeterminesOutputType = "OutputClass",ExpandBoolAsExecs = "bIsValid"))
	 static  UCancerActorPoolComponent* K2_GetCancerActorPoolComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerActorPoolComponent> OutputClass = nullptr);
};
