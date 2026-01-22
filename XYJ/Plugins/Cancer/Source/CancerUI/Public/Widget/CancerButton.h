

#pragma once

#include "CoreMinimal.h"
#include "CancerUserWidget.h"
#include "Components/Button.h"
#include "CancerButton.generated.h"

/**
 * CancerButton
 */
UCLASS()
class CANCERUI_API UCancerButton : public UCancerUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
