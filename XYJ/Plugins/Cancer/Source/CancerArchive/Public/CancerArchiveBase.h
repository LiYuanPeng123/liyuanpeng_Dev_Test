#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Misc/Crc.h"
#include "CancerArchiveBase.generated.h"

USTRUCT(BlueprintType)
struct FCancerStructSave
{
	GENERATED_BODY()
	UPROPERTY()
	FString TypePath;
	UPROPERTY()
	FString Json;
	UPROPERTY()
	uint32 Crc = 0;
};


UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class CANCERARCHIVE_API UCancerArchiveBase : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Archive", DisplayName="存档游戏时长")
	float GameTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archive", DisplayName="存档时间")
	int64 SaveUnixSeconds = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archive", DisplayName="存档日期")
	FString SaveTime;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="结构体数据")
	TMap<FName, FCancerStructSave> Structs;
};
