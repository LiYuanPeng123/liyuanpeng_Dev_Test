

#include "EvaluatorFunctionLibrary.h"

double UEvaluatorFunctionLibrary::Evaluator_Double(UObject* ContextObject, FEvaluator Evaluator)
{
	return Evaluator.Evaluator_Double(ContextObject);
}

bool UEvaluatorFunctionLibrary::Evaluator_Bool(UObject* ContextObject, FEvaluator Evaluator)
{
	return Evaluator.Evaluator_Bool(ContextObject);
}

double UEvaluatorFunctionLibrary::Evaluator_Double_Pure(UObject* ContextObject, FEvaluator Evaluator)
{
	return Evaluator_Double(ContextObject, Evaluator);
}

bool UEvaluatorFunctionLibrary::Evaluator_Bool_Pure(UObject* ContextObject, FEvaluator Evaluator)
{
	return Evaluator_Bool(ContextObject, Evaluator);
}
