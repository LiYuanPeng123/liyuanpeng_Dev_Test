#pragma once

#include "CoreMinimal.h"
#include "CancerArchiveBase.h"
#include "CancerArchiveSummary.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/Object.h"
#include "CancerArchiveSetting.generated.h"

class UCancerArchiveBase;
class UCancerArchiveSummary;
class UCancerArchiveDataAsset;

UENUM(BlueprintType)
enum class EArchiveSaveMode : uint8
{
	OverwriteLatest UMETA(DisplayName="覆盖最近一次"),
	NewEntry UMETA(DisplayName="新增历史条目")
};

UCLASS(config=Game, DefaultConfig, DisplayName="Cancer存档")
class CANCERARCHIVE_API UCancerArchiveSetting : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    static UCancerArchiveSetting* Get();

    UPROPERTY(Config, EditAnywhere, Category="Archive", DisplayName="存档简介")
    TSoftClassPtr<UCancerArchiveSummary> DefaultArchiveSummaryClass = UCancerArchiveSummary::StaticClass();

    UPROPERTY(Config, EditAnywhere, Category="Archive", DisplayName="存档数据")
    TSoftClassPtr<UCancerArchiveBase> DefaultGameArchiveClass = UCancerArchiveBase::StaticClass();

    UPROPERTY(Config, EditAnywhere, Category="Archive", DisplayName="启用默认存档数据")
    bool bUseDefault;

    UPROPERTY(Config, EditAnywhere, Category="Archive", meta=(EditCondition="bUseDefault"), DisplayName="默认存档数据")
    TSoftObjectPtr<UCancerArchiveDataAsset> DefaultArchiveDataAsset;

    UPROPERTY(Config, EditAnywhere, Category="Archive", DisplayName="保存模式")
    EArchiveSaveMode SaveMode = EArchiveSaveMode::OverwriteLatest;

    UPROPERTY(Config, EditAnywhere, Category="Archive", DisplayName="最大保留条目数")
    int32 MaxEntries = 10;

    UPROPERTY(Config, EditAnywhere, Category="Archive", DisplayName="存档简介名")
    FString SummarySlotName = TEXT("ArchiveSummary");
    
    virtual FName GetCategoryName() const;
    virtual FName GetSectionName() const;
};

