

#pragma once

#include "CoreMinimal.h"
#include "Evaluator_Base.h"
#include "Evaluator_Bool.generated.h"

/**
 * 
 */
UCLASS()
class CANCEREVALUATOR_API UEvaluator_Bool : public UEvaluator_Base
{
	GENERATED_BODY()
	public:
	virtual double GetValue(UObject* WorldContextObject) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, DisplayName="参数值")
	bool Value = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, DisplayName="取反")
	bool bReverse = false;
	UFUNCTION(BlueprintNativeEvent)
	bool GetResult(UObject* WorldContextObject);
};
