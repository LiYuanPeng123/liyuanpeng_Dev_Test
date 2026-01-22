#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Evaluator_Base.generated.h"


class UObject;
class UWorld;

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class UEvaluator_Base : public UObject
{
    GENERATED_BODY()

public:
    virtual UWorld* GetWorld() const override;
    virtual double GetValue(UObject* WorldContextObject);
};
