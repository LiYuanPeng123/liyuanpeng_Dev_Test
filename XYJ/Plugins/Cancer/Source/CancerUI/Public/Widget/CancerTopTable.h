#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CancerTopTable.generated.h"

class UImage;
class UCancerText;
/**
 * 
 */
UCLASS()
class CANCERUI_API UCancerTopTable : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UCancerText* DisplayText = nullptr;
	UPROPERTY(meta=(BindWidget))
	UImage* SelectedImage = nullptr;
};
