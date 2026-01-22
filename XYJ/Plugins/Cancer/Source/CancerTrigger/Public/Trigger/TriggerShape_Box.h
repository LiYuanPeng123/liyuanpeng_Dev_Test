
#pragma once

#include "CoreMinimal.h"
#include "TriggerShape_Base.h"
#include "UObject/Object.h"
#include "TriggerShape_Box.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CANCERTRIGGER_API UTriggerShape_Box : public UTriggerShape_Base
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess))
	FVector Extent = FVector(100.0f, 100.0f, 100.0f);

	virtual TSubclassOf<UPrimitiveComponent> GetPrimitiveComponentClass_Implementation() const override;

	virtual void PostAddComponent_Implementation(UPrimitiveComponent* NewComponent) override;
};
