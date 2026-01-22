
#pragma once

#include "CoreMinimal.h"
#include "TriggerShape_Base.h"

#include "TriggerShape_Sphere.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CANCERTRIGGER_API UTriggerShape_Sphere : public UTriggerShape_Base
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess))
	float Radius = 100.0f;

	virtual TSubclassOf<UPrimitiveComponent> GetPrimitiveComponentClass_Implementation() const override;

	virtual void PostAddComponent_Implementation(UPrimitiveComponent* NewComponent) override;
};
