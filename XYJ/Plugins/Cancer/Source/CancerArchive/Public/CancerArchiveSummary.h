// 

#pragma once

#include "CoreMinimal.h"
#include "CancerArchiveTypes.h"
#include "GameFramework/SaveGame.h"
#include "CancerArchiveBlueprintLibrary.h"
#include "CancerArchiveSummary.generated.h"

/**
 * //存档简介
 */
UCLASS()
class CANCERARCHIVE_API UCancerArchiveSummary : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Version;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FCancerArchiveDetail> Files;

	//获取最近一次存档
	UFUNCTION()
	FCancerArchiveDetail GetLastDetail()
	{
		if (Files.Num() == 0)
		{
			return AddNewDetail();
		}
		return Files.Last();
	}

	//添加新的存档
	UFUNCTION()
	FCancerArchiveDetail AddNewDetail()
	{
		FCancerArchiveDetail Detail;
		Detail.FileName = TEXT("SaveCount") + FString::FromInt(Files.Num() + 1);
		Detail.CreateUnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
		Detail.CreateTime = UCancerArchiveBlueprintLibrary::FormatUnixSeconds(Detail.CreateUnixSeconds);
		Files.Add(Detail);
		return Detail;
	}
};
