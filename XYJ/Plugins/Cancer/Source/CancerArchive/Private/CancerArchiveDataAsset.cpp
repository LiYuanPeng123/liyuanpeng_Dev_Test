// 


#include "CancerArchiveDataAsset.h"

#include "CancerArchiveBase.h"
#include "CancerArchiveSetting.h"
#include "CancerArchiveSummary.h"
#include "Kismet/GameplayStatics.h"

void UCancerArchiveDataAsset::LoadArchiveToDataAsset()
{
	const FString ArchiveSummarySlot = TEXT("ArchiveSummary");
	if (UGameplayStatics::DoesSaveGameExist(ArchiveSummarySlot, 0))
	{
		if (auto ArchiveSummary = Cast<
			UCancerArchiveSummary>(UGameplayStatics::LoadGameFromSlot(ArchiveSummarySlot, 0)))
		{
			auto Loaded = Cast<UCancerArchiveBase>(
				UGameplayStatics::LoadGameFromSlot(ArchiveSummary->GetLastDetail().FileName, 0));
			Archive = Cast<UCancerArchiveBase>(StaticDuplicateObject(Loaded, this));
			return;
		}
	}
	UClass* ArchiveSummaryClass = GetDefault<UCancerArchiveSetting>()->DefaultArchiveSummaryClass.LoadSynchronous();
	if (auto ArchiveSummary = NewObject<UCancerArchiveSummary>(this, ArchiveSummaryClass))
	{
		auto Loaded = Cast<UCancerArchiveBase>(
			UGameplayStatics::LoadGameFromSlot(ArchiveSummary->GetLastDetail().FileName, 0));
		Archive = Cast<UCancerArchiveBase>(StaticDuplicateObject(Loaded, this));
		return;
	}
}

void UCancerArchiveDataAsset::WriteArchiveToFile()
{
	const FString ArchiveSummarySlot = TEXT("ArchiveSummary");
	if (UGameplayStatics::DoesSaveGameExist(ArchiveSummarySlot, 0))
	{
		if (auto ArchiveSummary = Cast<
			UCancerArchiveSummary>(UGameplayStatics::LoadGameFromSlot(ArchiveSummarySlot, 0)))
		{
			UGameplayStatics::SaveGameToSlot(Archive, ArchiveSummary->GetLastDetail().FileName, 0);
			return;
		}
	}
	UClass* ArchiveSummaryClass = GetDefault<UCancerArchiveSetting>()->DefaultArchiveSummaryClass.LoadSynchronous();
	if (auto ArchiveSummary = NewObject<UCancerArchiveSummary>(this, ArchiveSummaryClass))
	{
		UGameplayStatics::SaveGameToSlot(Archive, ArchiveSummary->GetLastDetail().FileName, 0);
		return;
	}
}
