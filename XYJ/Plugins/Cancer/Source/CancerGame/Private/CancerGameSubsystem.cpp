

#include "CancerGameSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "CancerGameSetting.h"

void UCancerGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 清空现有数据
	TextDescriptions.Empty();
	GameImages.Empty();
	
	// 加载所有数据表
	LoadAllDataTables();
}

void UCancerGameSubsystem::LoadAllDataTables()
{
	if (const UCancerGameSetting* GameSetting = UCancerGameSetting::Get())
	{
		// 加载文本数据表
		for (UDataTable* DataTable : GameSetting->StrDataTables)
		{
			if (DataTable)
			{
				LoadDataTable(DataTable, true);
			}
		}

		// 加载图片数据表
		for (UDataTable* DataTable : GameSetting->ImageDataTables)
		{
			if (DataTable)
			{
				LoadDataTable(DataTable, false);
			}
		}
	}
}

void UCancerGameSubsystem::LoadDataTable(UDataTable* DataTable, bool bIsTextTable)
{
	if (!DataTable)
	{
		return;
	}

	if (bIsTextTable)
	{
		// 加载文本数据
		TArray<FCancerStrStruct*> Rows;
		DataTable->GetAllRows<FCancerStrStruct>(TEXT("LoadTextDataTable"), Rows);

		for (const FCancerStrStruct* Row : Rows)
		{
			if (Row)
			{
				TextDescriptions.Add(Row->Ungird, Row->Name);
			}
		}
	}
	else
	{
		// 加载图片数据
		TArray<FCancerImageStruct*> Rows;
		DataTable->GetAllRows<FCancerImageStruct>(TEXT("LoadImageDataTable"), Rows);

		for (const FCancerImageStruct* Row : Rows)
		{
			if (Row && Row->Texture2D.IsValid())
			{
				GameImages.Add(Row->Ungird, Row->Texture2D);
			}
		}
	}
}

FString UCancerGameSubsystem::GetTextDescription(int32 ID)
{
	if (const FString* FoundText = TextDescriptions.Find(ID))
	{
		return *FoundText;
	}
	return FString::Printf(TEXT("Missing Text ID: %d"), ID);
}

TSoftObjectPtr<UTexture2D> UCancerGameSubsystem::GetImage(int32 ID)
{
	if (const TSoftObjectPtr<UTexture2D>* FoundImage = GameImages.Find(ID))
	{
		return *FoundImage;
	}
	return nullptr;
}
