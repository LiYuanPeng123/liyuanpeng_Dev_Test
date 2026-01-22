#include "CancerArchiveSubsystem.h"
#include "CancerArchiveBase.h"
#include "CancerArchiveDataAsset.h"
#include "CancerArchiveSetting.h"
#include "CancerArchiveSummary.h"
#include "CancerArchiveBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

UE_DISABLE_OPTIMIZATION


DEFINE_LOG_CATEGORY_STATIC(LogArchive, Log, All);

TStatId UCancerArchiveSubsystem::GetStatId() const
{
	return TStatId();
}

void UCancerArchiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	auto bLoad = LoadGameArchive();

	FString Txt = bLoad ? TEXT("存档加载 -- 成功") : TEXT("存档加载 -- 失败");
	UE_LOG(LogArchive, Warning, TEXT("%s"), *Txt);
}

void UCancerArchiveSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UCancerArchiveSummary* UCancerArchiveSubsystem::GetArchiveSummary(
	TSubclassOf<UCancerArchiveSummary> ArchiveSummaryClass)
{
	return GetArchiveSummary();
}

UCancerArchiveBase* UCancerArchiveSubsystem::GetArchive(TSubclassOf<UCancerArchiveBase> ArchiveClass, bool bTransient)
{
	return GetArchive(bTransient);
}

void UCancerArchiveSubsystem::Tick(float DeltaTime)
{
	if (CurrentArchive)
		CurrentArchive->GameTime += DeltaTime;
}

UCancerArchiveSummary* UCancerArchiveSubsystem::LoadArchiveSummary()
{
	if (UGameplayStatics::DoesSaveGameExist(UCancerArchiveBlueprintLibrary::GetSummarySlotName(), 0))
	{
		ArchiveSummary = Cast<UCancerArchiveSummary>(
			UGameplayStatics::LoadGameFromSlot(UCancerArchiveBlueprintLibrary::GetSummarySlotName(), 0));
		if (ArchiveSummary)
		{
			return ArchiveSummary;
		}
	}
	UClass* ArchiveSummaryClass = GetDefault<UCancerArchiveSetting>()->DefaultArchiveSummaryClass.LoadSynchronous();
	ArchiveSummary = NewObject<UCancerArchiveSummary>(this, ArchiveSummaryClass);
	return ArchiveSummary;
}

bool UCancerArchiveSubsystem::LoadGameArchive()
{
	if (!ArchiveSummary)
	{
		LoadArchiveSummary();
	}
	//新建存档类
	NewGameArchive();

	//读取默认存档
	if (UCancerArchiveSetting::Get()->bUseDefault)
	{
		if (const UCancerArchiveDataAsset* LoadArchive = UCancerArchiveSetting::Get()->DefaultArchiveDataAsset.
			LoadSynchronous())
		{
			if (LoadArchive->Archive)
			{
				CurrentArchive = Cast<UCancerArchiveBase>(StaticDuplicateObject(LoadArchive->Archive, this));
				return true;
			}
		}
	}

	//如果存档文件存在则加载
	if (UGameplayStatics::DoesSaveGameExist(ArchiveSummary->GetLastDetail().FileName, 0))
	{
		CurrentArchive = Cast<UCancerArchiveBase>(
			UGameplayStatics::LoadGameFromSlot(ArchiveSummary->GetLastDetail().FileName, 0));
		if (CurrentArchive)
		{
			return true;
		}
	}
	return false;
}

void UCancerArchiveSubsystem::NewGameArchive()
{
	UClass* GameArchiveClass = GetDefault<UCancerArchiveSetting>()->DefaultGameArchiveClass.LoadSynchronous();
	CurrentArchive = NewObject<UCancerArchiveBase>(this, GameArchiveClass);
	TransientArchive = NewObject<UCancerArchiveBase>(this, GameArchiveClass);
}

void UCancerArchiveSubsystem::AsyncLoadGameArchive(const FAsyncLoadGameFromSlotDynamicDelegate& Delegate)
{
	if (!ArchiveSummary)
	{
		LoadArchiveSummary();
	}
	UGameplayStatics::AsyncLoadGameFromSlot(LoadArchiveSummary()->GetLastDetail().FileName, 0,
	                                        FAsyncLoadGameFromSlotDelegate::CreateLambda(
		                                        [this, Delegate](const FString& InSlotName, const int32 InUserIndex,
		                                                         USaveGame* InSaveGame)
		                                        {
			                                        UClass* GameArchiveClass = GetDefault<UCancerArchiveSetting>()->
			                                                                   DefaultGameArchiveClass.
			                                                                   LoadSynchronous();
			                                        TransientArchive = NewObject<UCancerArchiveBase>(
				                                        this, GameArchiveClass);
			                                        CurrentArchive = Cast<UCancerArchiveBase>(InSaveGame);
			                                        Delegate.ExecuteIfBound(InSlotName, InUserIndex, CurrentArchive);
		                                        }));
}


void UCancerArchiveSubsystem::AsyncWritingGameArchive(const FAsyncSaveGameFromSlotDynamicDelegate& Delegate)
{
	if (bIsBusy)
	{
		UE_LOG(LogArchive, Warning, TEXT("存档写入被拒绝：系统忙碌"));
		return;
	}
	if (!ArchiveSummary)
	{
		LoadArchiveSummary();
	}
	if (!CurrentArchive)
	{
		LoadGameArchive();
	}
	bIsBusy = true;

	//写入存档时间
	CurrentArchive->SaveUnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
	CurrentArchive->SaveTime = UCancerArchiveBlueprintLibrary::FormatUnixSeconds(CurrentArchive->SaveUnixSeconds);

	FCancerArchiveDetail TargetDetail;
	if (UCancerArchiveSetting::Get()->SaveMode == EArchiveSaveMode::NewEntry)
	{
		TargetDetail = ArchiveSummary->AddNewDetail();
	}
	else
	{
		TargetDetail = ArchiveSummary->GetLastDetail();
	}

	UGameplayStatics::AsyncSaveGameToSlot(CurrentArchive, TargetDetail.FileName, 0,
	                                      FAsyncSaveGameToSlotDelegate::CreateWeakLambda(this,
		                                      [this, Delegate](const FString& SaveGameSlotName,
		                                                       const int32 SaveGameUserIndex,
		                                                       const bool SaveGameSuccess)
		                                      {
			                                      if (SaveGameSuccess)
			                                      {
				                                      //裁剪历史条目超出限制
				                                      const int32 MaxKeep = UCancerArchiveSetting::Get()->MaxEntries;
				                                      while (ArchiveSummary->Files.Num() > MaxKeep && ArchiveSummary->
					                                      Files.Num() > 0)
				                                      {
					                                      const FCancerArchiveDetail Oldest = ArchiveSummary->Files[0];
					                                      UGameplayStatics::DeleteGameInSlot(Oldest.FileName, 0);
					                                      ArchiveSummary->Files.RemoveAt(0);
				                                      }
				                                      UGameplayStatics::SaveGameToSlot(
					                                      ArchiveSummary,
					                                      UCancerArchiveBlueprintLibrary::GetSummarySlotName(), 0);
			                                      }

			                                      Delegate.ExecuteIfBound(
				                                      SaveGameSlotName, SaveGameUserIndex, SaveGameSuccess);
			                                      bIsBusy = false;
		                                      }));
}

UE_ENABLE_OPTIMIZATION
