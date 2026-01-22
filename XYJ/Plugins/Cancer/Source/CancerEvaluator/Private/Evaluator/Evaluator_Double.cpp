
#include"Evaluator/Evaluator_Double.h"

#include "EvaluatorType.h"

double UEvaluator_Double::GetValue(UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return GetResult(WorldContextObject);
	}
	UE_LOG(LogEvaluator, Warning, TEXT("上下文为空,使用模拟值计算%s"), *GetName())
	return Value;
}

double UEvaluator_Double::GetResult_Implementation(UObject* WorldContextObject)
{
	return Value;
}
