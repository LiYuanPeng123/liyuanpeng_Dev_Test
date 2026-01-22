
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameSettingVisualData.generated.h"

class UGameSetting;
class UGameSettingListEntryBase;
/**
 * 
 */
UCLASS(BlueprintType)
class CANCERGAMESETTING_API UGameSettingVisualData : public UDataAsset
{
	GENERATED_BODY()
public:
	TSubclassOf<UGameSettingListEntryBase> GetEntryForSetting(UGameSetting* InSetting);

protected:
	UPROPERTY(EditDefaultsOnly, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSubclassOf<UGameSetting>, TSubclassOf<UGameSettingListEntryBase>> EntryWidgetForClass;
};
