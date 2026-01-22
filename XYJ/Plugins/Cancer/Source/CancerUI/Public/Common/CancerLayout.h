#pragma once

#include "CoreMinimal.h"
#include "CancerUserWidget.h"
#include "GameplayTagContainer.h"
#include "CancerLayout.generated.h"


class UCancerWidgetStack;

UCLASS(Abstract, meta = (DisableNativeTick))
class CANCERUI_API UCancerLayout : public UCancerUserWidget
{
	GENERATED_BODY()

public:
	static UCancerLayout* GetPrimaryGameLayout(const UObject* WorldContextObject);

	UCancerLayout(const FObjectInitializer& ObjectInitializer);
	
	UCancerWidgetStack* GetLayerWidget(FGameplayTag LayerName);
	
	void AddWidget(FGameplayTag LayerTag,UCancerUserWidget* Widget);
	
	void RemoveWidget(UCancerUserWidget*Widget);
	
protected:
	UFUNCTION(BlueprintCallable, Category="Layer")
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer"))
	                   FGameplayTag LayerTag, UCancerWidgetStack* LayerWidget);
private:
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCancerWidgetStack>> Layers;
};
