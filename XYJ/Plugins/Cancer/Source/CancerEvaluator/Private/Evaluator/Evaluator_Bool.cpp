
#include"Evaluator/Evaluator_Bool.h"

#include "EvaluatorType.h"

double UEvaluator_Bool::GetValue(UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		if (bReverse)
		{
			return !GetResult(WorldContextObject);
		}
		return GetResult(WorldContextObject);
	}
	UE_LOG(LogEvaluator, Warning, TEXT("上下文为空,使用模拟值计算%s"), *GetName())
	if (bReverse)
	{
		return !Value;
	}
	return Value;
}

bool UEvaluator_Bool::GetResult_Implementation(UObject* WorldContextObject)
{
	return Value;
}
