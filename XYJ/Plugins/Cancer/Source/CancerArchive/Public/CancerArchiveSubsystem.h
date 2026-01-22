#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "CancerArchiveSubsystem.generated.h"


class UCancerArchiveBase;
class UCancerArchiveSummary;

UCLASS(Blueprintable)
class CANCERARCHIVE_API UCancerArchiveSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

private:
	//是否阻塞
	UPROPERTY()
	bool bIsBusy = false;

	//当前已经加载了的存档,会被实例化
	UPROPERTY()
	UCancerArchiveBase* CurrentArchive;

	//临时存档,不会被实例化
	UPROPERTY()
	UCancerArchiveBase* TransientArchive;

	// 存档简介
	/**
	 * 
	 */
	UPROPERTY()
	UCancerArchiveSummary* ArchiveSummary;

public:


	virtual TStatId GetStatId() const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void Deinitialize() override;
	
	static UCancerArchiveSubsystem* Get(const UObject* WorldContextObject)
	{
		if (!WorldContextObject) return nullptr;
		UWorld* World = WorldContextObject->GetWorld();
		if (!World) return nullptr;
		UGameInstance* GI = World->GetGameInstance();
		return GI ? GI->GetSubsystem<UCancerArchiveSubsystem>() : nullptr;
	}

	//获取存档简介
	UFUNCTION(BlueprintPure, meta = (ComponentClass = "/Script/Engine.SaveGame"),
		meta = (DeterminesOutputType = "ArchiveClass"))
	UCancerArchiveSummary* GetArchiveSummary(TSubclassOf<UCancerArchiveSummary> ArchiveSummaryClass);

	template <typename T = UCancerArchiveSummary>
	T* GetArchiveSummary()
	{
		if (!ArchiveSummary)
		{
			LoadArchiveSummary();
		}
		return Cast<T>(ArchiveSummary);
	}
	
	//获取存档
	UFUNCTION(BlueprintPure, meta = (ComponentClass = "/Script/Engine.SaveGame"),
		meta = (DeterminesOutputType = "ArchiveClass"))
	UCancerArchiveBase* GetArchive(TSubclassOf<UCancerArchiveBase> ArchiveClass, bool bTransient = false);

	template <typename T = UCancerArchiveBase>
	T* GetArchive(bool bTransient)
	{
		if (!CurrentArchive)
		{
			NewGameArchive();
		}
		if (bTransient)
		{
			return Cast<T>(TransientArchive);
		}
		else
		{
			return Cast<T>(CurrentArchive);
		}
	}
	
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FAsyncLoadGameFromSlotDynamicDelegate, const FString&, InSlotName, const int32,
										 UserIndex, UCancerArchiveBase*, InSaveGame);

	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FAsyncSaveGameFromSlotDynamicDelegate, const FString&, InSlotName, const int32,
										 UserIndex, bool, Insuccess);

	//加载存档简介
	UCancerArchiveSummary* LoadArchiveSummary();
	
	//加载存档
	bool LoadGameArchive();

	//新建存档
	void NewGameArchive();

	//异步加载存档
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Delegate"))
	void AsyncLoadGameArchive(const FAsyncLoadGameFromSlotDynamicDelegate& Delegate);
	
	//异步写入存档
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Delegate"))
	void AsyncWritingGameArchive(const FAsyncSaveGameFromSlotDynamicDelegate& Delegate);

	//是否忙碌
	UFUNCTION(BlueprintPure)
	bool IsBusy() const { return bIsBusy; }
	
	
	

	
};
