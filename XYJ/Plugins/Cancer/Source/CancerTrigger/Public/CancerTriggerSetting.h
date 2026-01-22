
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerTriggerSetting.generated.h"

UCLASS(config = Game,DefaultConfig,DisplayName="Trigger")
class CANCERTRIGGER_API UCancerTriggerSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerTriggerSetting* Get();
	
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "默认触发器碰撞预设"))
	FCollisionProfileName TriggerProfileName;

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
