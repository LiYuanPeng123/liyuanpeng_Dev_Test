
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerActorPoolSetting.generated.h"

UCLASS(Config=Game,DefaultConfig,DisplayName="CancerActorPool")
class CANCERACTORPOOL_API UCancerActorPoolSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerActorPoolSetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
