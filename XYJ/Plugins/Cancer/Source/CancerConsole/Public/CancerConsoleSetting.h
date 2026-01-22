#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerConsoleSetting.generated.h"

UCLASS(Config= Game, DefaultConfig, DisplayName="控制台命令")
class CANCERCONSOLE_API UCancerConsoleSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UCancerConsoleSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
