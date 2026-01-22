
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerIndicatorSetting.generated.h"

UCLASS(Config=Game,DefaultConfig,DisplayName="CancerIndicator")
class CANCERINDICATOR_API UCancerIndicatorSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerIndicatorSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
