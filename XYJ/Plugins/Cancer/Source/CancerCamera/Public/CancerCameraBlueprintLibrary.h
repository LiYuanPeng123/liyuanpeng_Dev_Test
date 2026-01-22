#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerCameraBlueprintLibrary.generated.h"

class UCancerCameraBehaviorTreeComponent;

UCLASS()
class CANCERCAMERA_API UCancerCameraBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintPure, Category="CancerCamera")
    static FMinimalViewInfo GetCameraCacheView(APlayerCameraManager* CameraManager);

    template <class T = UCancerCameraBehaviorTreeComponent>
    static T* GetCancerCameraBehaviorTreeComponent(AActor* Actor)
    {
        if (!IsValid(Actor)) return nullptr;
        return Actor->GetComponentByClass<T>();
    }

    UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="GetCancerCameraBehaviorTreeComponent",
        meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
    static UCancerCameraBehaviorTreeComponent* K2_GetCancerCameraBehaviorTreeComponent(
        AActor* Actor, bool& bIsValid, TSubclassOf<UCancerCameraBehaviorTreeComponent> OutputClass = nullptr);
    
};