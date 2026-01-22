

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CancerAssetSetting.generated.h"

UCLASS(Config = Game, DefaultConfig,DisplayName="CancerAsset")
class CANCERASSET_API UCancerAssetSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="Action资产路径", LongPackageName))
	TArray<FDirectoryPath> ItemTableDataAssetPathArray;

	static UCancerAssetSetting* Get();
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
