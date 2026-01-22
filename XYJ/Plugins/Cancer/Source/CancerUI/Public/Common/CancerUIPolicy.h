
#pragma once
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "Types/UIDefinition.h"

#include "CancerUIPolicy.generated.h"


class UCommonActivatableWidget;
class UCancerUIManagerSubsystem;
class UCancerLayout;

UCLASS(Abstract, Blueprintable)
class CANCERUI_API UCancerUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	template <typename GameUIPolicyClass = UCancerUIPolicy>
	static GameUIPolicyClass* GetGameUIPolicyAs(const UObject* WorldContextObject)
	{
		return Cast<GameUIPolicyClass>(GetGameUIPolicy(WorldContextObject));
	}

	static UCancerUIPolicy* GetGameUIPolicy(const UObject* WorldContextObject);

	virtual UWorld* GetWorld() const override;
	
	UCancerUIManagerSubsystem* GetOwningUIManager() const;

	UCancerLayout* GetRootLayout(UCancerUIManagerSubsystem* UIManager);

	TMap<FGameplayTag, FUIInfo> GetUIInfoMap() const
	{
		TMap<FGameplayTag, FUIInfo> InfoMap;

		if (DataTables.IsEmpty()) return InfoMap;
		
		for (auto Data : DataTables)
		{
			TArray<FUIInfo*> InfoArray;
			if (!Data) continue;
			Data->GetAllRows(FString(), InfoArray);
			for (const auto& Info : InfoArray)
			{
				InfoMap.Add(Info->UITag, *Info);
			}
		}
		return InfoMap;
	}
	
	void AddWidget(FGameplayTag LayoutTag,UCancerUserWidget* Widget);
	void RemoveWidget(UCancerUserWidget*Widget);
	
protected:
	UPROPERTY(EditAnywhere,DisplayName="UI层")
	TSoftClassPtr<UCancerLayout> LayoutClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		DisplayName="UI管理表", meta=(AllowPrivateAccess = true, Categories="UI",
			RequiredAssetDataTags = "RowStructure=/Script/CancerUI.UIInfo"))
	TArray<UDataTable*> DataTables;

private:
	UPROPERTY()
	UCancerLayout* RootLayout;

	friend class UCancerUIManagerSubsystem;
};
