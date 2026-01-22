
#pragma once

#include "CoreMinimal.h"
#include "EvaluatorType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EvaluatorFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CANCEREVALUATOR_API UEvaluatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static double Evaluator_Double(UObject* ContextObject, FEvaluator Evaluator);
	UFUNCTION(BlueprintCallable)
	static bool Evaluator_Bool(UObject* ContextObject, FEvaluator Evaluator);

	UFUNCTION(BlueprintPure)
	static double Evaluator_Double_Pure(UObject* ContextObject, FEvaluator Evaluator);
	UFUNCTION(BlueprintPure)
	static bool Evaluator_Bool_Pure(UObject* ContextObject, FEvaluator Evaluator);
};
