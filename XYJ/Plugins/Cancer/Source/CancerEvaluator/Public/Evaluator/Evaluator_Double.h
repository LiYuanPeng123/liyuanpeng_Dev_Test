
#pragma once

#include "CoreMinimal.h"
#include "Evaluator_Base.h"
#include "Evaluator_Double.generated.h"

/**
 * 
 */
UCLASS()
class CANCEREVALUATOR_API UEvaluator_Double : public UEvaluator_Base
{
	GENERATED_BODY()
public:
	virtual double GetValue(UObject* WorldContextObject) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, DisplayName="参数值")
	double Value;
	
	UFUNCTION(BlueprintNativeEvent)
	double GetResult(UObject* WorldContextObject);
	
};
