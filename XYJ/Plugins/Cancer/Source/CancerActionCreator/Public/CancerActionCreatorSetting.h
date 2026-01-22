
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerActionCreatorSetting.generated.h"

UCLASS(config=Game,DefaultConfig,DisplayName="CancerActionCreattor")
class CANCERACTIONCREATOR_API UCancerActionCreatorSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerActionCreatorSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
