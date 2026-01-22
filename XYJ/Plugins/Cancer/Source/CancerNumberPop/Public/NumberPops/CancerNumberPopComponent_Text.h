
#pragma once

#include "CoreMinimal.h"
#include "CancerNumberPopComponent.h"
#include "CancerPopText.h"
#include "CancerNumberPopComponent_Text.generated.h"


UCLASS(ClassGroup=(Cancer), meta=(BlueprintSpawnableComponent))
class CANCERNUMBERPOP_API UCancerNumberPopComponent_Text : public UCancerNumberPopComponent
{
	GENERATED_BODY()

public:
	UCancerNumberPopComponent_Text();

	virtual void AddNumberPop(const FCancerNumberPopRequest& NewRequest) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Text)
	TSubclassOf<UCancerPopText> TextWidgetClass = UCancerPopText::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Text)
	FLinearColor DamageColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Text)
	FLinearColor CriticalDamageColor = FLinearColor::Red;
};
