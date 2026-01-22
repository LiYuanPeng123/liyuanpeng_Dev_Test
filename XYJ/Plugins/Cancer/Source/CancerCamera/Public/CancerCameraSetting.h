
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerCameraSetting.generated.h"

/**
 * 
 */
UCLASS(config = Game,DefaultConfig,DisplayName="CancerCamera")
class CANCERCAMERA_API UCancerCameraSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UCancerCameraSetting* Get();

	virtual FName GetCategoryName() const override;
};
