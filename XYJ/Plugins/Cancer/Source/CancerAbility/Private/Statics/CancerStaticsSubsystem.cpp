// Fill out your copyright notice in the Description page of Project Settings.


#include "Statics/CancerStaticsSubsystem.h"

#include "Statics/CancerStaticsSetting.h"

void UCancerStaticsSubsystem::Reset()
{
	CharacterStaticsInfo.Reset();
}

void UCancerStaticsSubsystem::StaticDamage(AActor* Target, float InDamage)
{
	if (!IsValid(Target)) return;
	if (InDamage <= 0 )
	{
		UE_LOG(LogTemp, Warning , TEXT("UCancerStaticsSubsystem    Damage Small than 0 "))
	}
	
	if (UCancerStaticsSubsystem* StaticsSubsystem = Target->GetWorld()->GetGameInstance()->GetSubsystem<UCancerStaticsSubsystem>())
	{
		auto& StaticsInfo =  StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target);
		StaticsInfo.TotalDamage += InDamage;
		const UCancerStaticsSetting* Settings = GetDefault<UCancerStaticsSetting>();
		Target->GetWorld()->GetTimerManager().SetTimer(StaticsSubsystem->DamageStaticsTimerHandle,[Target,StaticsSubsystem]()
		{
			StaticsSubsystem->CharacterStaticsInfo[Target].TotalDamage = 0.f;
			
		},Settings->DamageResetTime, false);
	}
}

float UCancerStaticsSubsystem::GetTotalDamage(AActor* Target)
{
	return CharacterStaticsInfo.FindOrAdd(Target).TotalDamage;
}

void UCancerStaticsSubsystem::StaticStagger(AActor* Target)
{
	if (!IsValid(Target)) return;
	
	if (UCancerStaticsSubsystem* StaticsSubsystem = Target->GetWorld()->GetGameInstance()->GetSubsystem<UCancerStaticsSubsystem>())
	{
		auto& StaticsInfo =  StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target);
		StaticsInfo.TotalStagger += 1;
		const UCancerStaticsSetting* Settings = GetDefault<UCancerStaticsSetting>();
		Target->GetWorld()->GetTimerManager().SetTimer(StaticsSubsystem->StaggerStaticsTimerHandle,[Target,StaticsSubsystem]()
		{
			StaticsSubsystem->CharacterStaticsInfo[Target].TotalStagger = 0;
		},Settings->StaggerResetTime, false);
	}
}

int32 UCancerStaticsSubsystem::GetTotalStagger(AActor* Target)
{
	return CharacterStaticsInfo.FindOrAdd(Target).TotalStagger;
}

void UCancerStaticsSubsystem::StaticBlock(AActor* Target)
{
	if (!IsValid(Target)) return;
	
	if (UCancerStaticsSubsystem* StaticsSubsystem = Target->GetWorld()->GetGameInstance()->GetSubsystem<UCancerStaticsSubsystem>())
	{
		auto& StaticsInfo =  StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target);
		StaticsInfo.TotalBlock += 1;
		
		const UCancerStaticsSetting* Settings = GetDefault<UCancerStaticsSetting>();
		Target->GetWorld()->GetTimerManager().SetTimer(StaticsSubsystem->BlockStaticsTimerHandle,[Target,StaticsSubsystem]()
		{
			StaticsSubsystem->CharacterStaticsInfo[Target].TotalBlock = 0;
		},Settings->BlockResetTime, false);
	}
}

int32 UCancerStaticsSubsystem::GetTotalBlock(AActor* Target)
{
	return CharacterStaticsInfo.FindOrAdd(Target).TotalBlock;
}

void UCancerStaticsSubsystem::StaticPerfectBlock(AActor* Target)
{
	if (!IsValid(Target)) return;
	
	if (UCancerStaticsSubsystem* StaticsSubsystem = Target->GetWorld()->GetGameInstance()->GetSubsystem<UCancerStaticsSubsystem>())
	{
		auto& StaticsInfo =  StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target);
		StaticsInfo.TotalPerfectBlock += 1;
		
		const UCancerStaticsSetting* Settings = GetDefault<UCancerStaticsSetting>();
		Target->GetWorld()->GetTimerManager().SetTimer(StaticsSubsystem->PerfectBlockStaticsTimerHandle,[Target,StaticsSubsystem]()
		{
			StaticsSubsystem->CharacterStaticsInfo[Target].TotalPerfectBlock = 0;
		},Settings->PerfectBlockResetTime, false);
	}
}

int32 UCancerStaticsSubsystem::GetTotalPerfectBlock(AActor* Target)
{
	return CharacterStaticsInfo.FindOrAdd(Target).TotalPerfectBlock;
}

void UCancerStaticsSubsystem::StaticBattleStart(AActor* Target)
{
	if (!IsValid(Target)) return;
	
	if (UCancerStaticsSubsystem* StaticsSubsystem = Target->GetWorld()->GetGameInstance()->GetSubsystem<UCancerStaticsSubsystem>())
	{
		auto& StaticsInfo =  StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target);
		if (!StaticsInfo.BattleTimerHandle.IsValid())
		{
			Target->GetWorld()->GetTimerManager().SetTimer(StaticsInfo.BattleTimerHandle,[StaticsSubsystem,Target]()
			{
				StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target).BattleTime += 0.2f;
			},0.2f,true);
		}
	}
}

void UCancerStaticsSubsystem::StaticBattleEnd(AActor* Target)
{
	if (!IsValid(Target)) return;
	
	if (UCancerStaticsSubsystem* StaticsSubsystem = Target->GetWorld()->GetGameInstance()->GetSubsystem<UCancerStaticsSubsystem>())
	{
		auto& StaticsInfo =  StaticsSubsystem->CharacterStaticsInfo.FindOrAdd(Target);
		if (StaticsInfo.BattleTimerHandle.IsValid())
		{
			Target->GetWorld()->GetTimerManager().ClearTimer(StaticsInfo.BattleTimerHandle);
			StaticsInfo.BattleTime  = 0.f;
		}
	}
}

double UCancerStaticsSubsystem::GetTotalBattleTime(AActor* Target)
{
	return CharacterStaticsInfo.FindOrAdd(Target).BattleTime;
}
