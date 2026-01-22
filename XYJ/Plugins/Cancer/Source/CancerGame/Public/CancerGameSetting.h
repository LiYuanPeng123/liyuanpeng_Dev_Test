#pragma once

#include "CoreMinimal.h"
#include "CancerGameSetting.generated.h"


USTRUCT(BlueprintType)
struct FCancerStrStruct : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="描述")
	FString Name = "";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="唯一标识")
	int32 Ungird = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="简介")
	FString Str = "该文本作用";
};

USTRUCT(BlueprintType)
struct FCancerImageStruct : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="图片")
	TSoftObjectPtr<UTexture2D> Texture2D = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="唯一标识")
	int32 Ungird = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="简介")
	FString Str = "该图片作用";
};

UCLASS(Config= Game, DefaultConfig, DisplayName="CancerGame")
class CANCERGAME_API UCancerGameSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		DisplayName="文本描述", meta=(AllowPrivateAccess = true, Categories="CancerGame",
			RequiredAssetDataTags = "RowStructure=/Script/CancerGame.CancerStrStruct"))
	TArray<UDataTable*> StrDataTables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		DisplayName="图片", meta=(AllowPrivateAccess = true, Categories="CancerGame",
			RequiredAssetDataTags = "RowStructure=/Script/CancerGame.CancerImageStruct"))
	TArray<UDataTable*> ImageDataTables;

	static UCancerGameSetting* Get();
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
