
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerNumberPopSetting.generated.h"

UCLASS(Config = Game,DefaultConfig,DisplayName= "CancerNumberPop")
class CANCERNUMBERPOP_API UCancerNumberPopSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerNumberPopSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
