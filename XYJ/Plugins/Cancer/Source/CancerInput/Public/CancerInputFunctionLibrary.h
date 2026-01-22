
#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerInputFunctionLibrary.generated.h"

class UInputAction;
struct FBufferedInputCommand;
struct FInputActionInstance;
enum class ETriggerEvent : uint8;
class UCancerInputManagerComponent;
class UCancerInputHandler;



UCLASS()
class CANCERINPUT_API UCancerInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	
	static FBufferedInputCommand MakeBufferedInputCommand(UCancerInputManagerComponent* Source,
		UCancerInputHandler* Handler, const FInputActionInstance& ActionInstance, ETriggerEvent TriggerEvent);
	
	UFUNCTION(BlueprintPure, Category = "CancerInputFunctionLibrary", meta = (BlueprintThreadSafe))
	static void BreakBufferedInputCommand(const FBufferedInputCommand& Command, bool& bIsValid,
		UCancerInputManagerComponent*& Source, UCancerInputHandler*& Handler, FInputActionInstance& ActionInstance,
		ETriggerEvent& TriggerEvent);

#pragma region //获取增强输入系统
	template <class T = UEnhancedInputComponent>
	static T* GetInputComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		if (AController* Controller = Cast<AController>(Actor))
		{
			if (APawn* Pawn = Controller->GetPawn())
			{
				return Cast<T>(Pawn->InputComponent);
			}
		}
		return Cast<T>(Actor->InputComponent);
	}
	UFUNCTION(BlueprintCallable, Category = "CancerInputFunctionLibrary", DisplayName="Get Input Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UInputComponent* K2_GetInputComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UInputComponent> OutputClass = nullptr);
#pragma endregion


#pragma region //获取Cancer输入管理组件
	template <class T = UCancerInputManagerComponent>
	static T* GetCancerInputManagerComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;

		return Actor->GetComponentByClass<T>();
		
	}
	UFUNCTION(BlueprintCallable, Category = "CancerInputFunctionLibrary", DisplayName="Get Cancer Input Manager Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static class UCancerInputManagerComponent* K2_CancerInputManagerComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerInputManagerComponent> OutputClass = nullptr);
#pragma endregion

	
	
	UFUNCTION(BlueprintPure, Category = "Cancer Input")
	static UActorComponent* GetInputBufferComponent(const AActor* Actor);

	
	UFUNCTION(BlueprintCallable,meta=(WorldContext="WorldContextObject"))
	static FInputActionValue GetInputActionValue(UObject* WorldContextObject,UInputAction* InputAction);

	UFUNCTION(BlueprintCallable)
	static FInputActionInstance GetActionInstance(UObject* WorldContextObject,UInputAction* InputAction);
	
	
};
