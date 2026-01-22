

#include "CancerEvaluatorSetting.h"

UCancerEvaluatorSetting* UCancerEvaluatorSetting::Get()
{
	return GetMutableDefault<UCancerEvaluatorSetting>();
}

FName UCancerEvaluatorSetting::GetCategoryName() const
{
	return FName(TEXT("Evaluator"));
}

FName UCancerEvaluatorSetting::GetSectionName() const
{
	return FName(TEXT("CancerEvaluator"));
}
