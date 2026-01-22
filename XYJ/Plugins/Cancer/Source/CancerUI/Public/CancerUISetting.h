
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "CancerUISetting.generated.h"


class UCancerUIPolicy;

UCLASS(config = Game, DefaultConfig, DisplayName="CancerUI")
class CANCERUI_API UCancerUISetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCancerUISetting();
		UPROPERTY(Config, EditAnywhere, Category = "CancerUI",
    			meta = (DisplayName = "启用"))
    			bool bEnabled = true;
	UPROPERTY(Config, EditAnywhere, Category = "CancerUI",
			meta = (DisplayName = "默认GameUIPolicy设置"))
	TSoftClassPtr<UCancerUIPolicy> DefaultUIPolicyClass;
	
	UPROPERTY(Config, EditAnywhere, Category = "CancerUI",
		meta = (DisplayName = "图层排序", Categories="UI.Layer"))
	TArray<FGameplayTag> LayerSort;

	UPROPERTY(Config, EditAnywhere, Category = "CancerUI",
		meta = (DisplayName = "UI输入层"))
	FGameplayTag UIInputTag;
	
	static UCancerUISetting* Get();

	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
