

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerEvaluatorSetting.generated.h"

UCLASS(config = Game,DefaultConfig,DisplayName="Evaluator")
class CANCEREVALUATOR_API UCancerEvaluatorSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerEvaluatorSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
