
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/UIDefinition.h"
#include "CancerUIFunctionLibrary.generated.h"


class UInputAction;
class UCancerUserWidget;
class UCommonActivatableWidget;

UCLASS()
class CANCERUI_API UCancerUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UCancerUIFunctionLibrary() { }

	UFUNCTION(BlueprintCallable, Category = "Global UI Extensions", meta = (WorldContext = "WorldContextObject", DeterminesOutputType="Class"))
	static UCancerUserWidget* GetUI(UObject* WorldContextObject,UPARAM(meta = (Categories = "UI"))
		FGameplayTag UIName, TSubclassOf<UCancerUserWidget> Class = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Global UI Extensions", meta = (WorldContext = "WorldContextObject", DeterminesOutputType="Class"))
	static UCancerUserWidget* OpenUI(UObject* WorldContextObject,UPARAM(meta = (Categories = "UI"))
		FGameplayTag UIName, TSubclassOf<UCancerUserWidget> Class = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Global UI Extensions", meta = (WorldContext = "WorldContextObject"))
	static void CloseUI(UObject* WorldContextObject,UPARAM(meta = (Categories = "UI"))
		FGameplayTag UIName, bool bDestroy = false, UCancerUserWidget* InWidget = nullptr);


	UFUNCTION(BlueprintCallable, Category = "Global UI Extensions", meta = (WorldContext = "WorldContextObject", DeterminesOutputType="Class"))
	static UCancerUserWidget* OpenUIWithData(UObject* WorldContextObject,UPARAM(meta = (Categories = "UI"))
	FGameplayTag UIName, TSubclassOf<UCancerUserWidget> Class = nullptr,UObject*Payload = nullptr);


	UFUNCTION(BlueprintCallable, Category = "Global UI Extensions", meta = (WorldContext = "WorldContextObject", DeterminesOutputType="Class"))
	static UCancerUserWidget* CloseUIWithData(UObject* WorldContextObject, FGameplayTag UIName, bool bDestroy,
	UCancerUserWidget* InWidget,UObject*Payload = nullptr);
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static void FocusTargetWidget(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions",meta=(WorldContext="WorldContextObject"))
	static EDirectionType GetWASD (UObject* WorldContextObject,UInputAction*InputAction);
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions",meta=(WorldContext="WorldContextObject"))
	static FVector GetInputActionValue (UObject* WorldContextObject,UInputAction*InputAction);
	
};
