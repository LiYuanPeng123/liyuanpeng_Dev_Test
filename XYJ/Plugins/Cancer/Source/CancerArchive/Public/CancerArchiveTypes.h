#pragma once

#include "CoreMinimal.h"
#include "CancerArchiveTypes.generated.h"


//存档结构
USTRUCT(BlueprintType)
struct CANCERARCHIVE_API FCancerArchiveDetail
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archive", DisplayName="存档插槽")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="存档文件")
	FString FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archive", DisplayName="创建时间")
	int64 CreateUnixSeconds = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archive", DisplayName="创建日期")
	FString CreateTime;
};

