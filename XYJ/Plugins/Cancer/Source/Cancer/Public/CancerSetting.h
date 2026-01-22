
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerSetting.generated.h"

UCLASS(Config=Game,DefaultConfig,DisplayName="Cancer")
class CANCER_API UCancerSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
	
};
