

#pragma once

#include "CoreMinimal.h"
#include "FLoadingScreenSetting.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "CancerLoadingScreenSetting.generated.h"

UCLASS(Config=Game,DefaultConfig,DisplayName="Cancer加载屏幕")
class CANCERLOADINGSCREEN_API UCancerLoadingScreenSetting : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
public:

	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="启用加载界面"))
	bool bEnableLoadingScreen;

	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="开始加载界面设置", EditCondition = bEnableLoadingScreen))
	FLoadingScreenSetting StartLoadingScreenSetting;
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="默认加载界面设置", EditCondition = bEnableLoadingScreen))
	FLoadingScreenSetting DefaultLoadingScreenSetting;
	
	UCancerLoadingScreenSetting();
	static UCancerLoadingScreenSetting* Get();
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
