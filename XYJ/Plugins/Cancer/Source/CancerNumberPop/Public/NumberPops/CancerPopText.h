
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CancerPopText.generated.h"

/**
 * 
 */
UCLASS()
class CANCERNUMBERPOP_API UCancerPopText : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Cancer Pop Text")
	void SetTextValue(int32 Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Cancer Pop Text")
	void SetTextColor(FLinearColor Color);
};
