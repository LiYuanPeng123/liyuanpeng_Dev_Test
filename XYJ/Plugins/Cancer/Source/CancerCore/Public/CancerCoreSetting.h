
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerCoreSetting.generated.h"

class UGameplayEffect;

UCLASS(Config = Game,DefaultConfig,DisplayName="CancerCore")
class CANCERCORE_API UCancerCoreSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UCancerCoreSetting* Get();
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
	
	
	// 技能配置表
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	TSoftObjectPtr<UDataTable> AbilityConfigTableRef;
};
