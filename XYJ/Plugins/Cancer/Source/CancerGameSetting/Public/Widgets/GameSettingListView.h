
#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"

#include "GameSettingListView.generated.h"

class UGameSetting;
class UGameSettingListEntryBase;
class UGameSettingCollection;
class UGameSettingVisualData;
/**
 * 设置列表UI组件
 */
UCLASS(meta = (EntryClass = GameSettingListEntryBase))
class CANCERGAMESETTING_API UGameSettingListView : public UListView
{
	GENERATED_BODY()

public:
	/**
	 * 设置设置项集合
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetSettingCollection(UGameSettingCollection* InCollection);
	
	UGameSettingListView(const FObjectInitializer& ObjectInitializer);

	void AddNameOverride(const FName& DevName, const FText& OverrideName);

	// 手柄导航接口
	UFUNCTION(BlueprintCallable, Category = "导航")
	void NavigateUp();
	
	UFUNCTION(BlueprintCallable, Category = "导航")
	void NavigateDown();
	
	UFUNCTION(BlueprintCallable, Category = "导航")
	void SelectFirstItem();
	
	UFUNCTION(BlueprintCallable, Category = "导航")
	void SelectItemByIndex(int32 Index);
	
	UFUNCTION(BlueprintPure, Category = "导航")
	int32 GetSelectedIndex() const;
	UFUNCTION(BlueprintCallable)
	UGameSetting* GetCurrentSelectedSetting() const;
	UFUNCTION(BlueprintCallable)
	UGameSettingListEntryBase* GetCurrentSelectedEntryWidget() const;


	UFUNCTION(BlueprintCallable)
	bool AdjustCurrentScalarSetting(float DeltaValue);
	UFUNCTION(BlueprintCallable)
	bool AdjustCurrentDiscreteSetting(bool bIncrease);

	UFUNCTION(BlueprintPure, Category = "导航")
	bool GetEnableHoverSelection()const {return bEnableHoverSelection;}

#if WITH_EDITOR
	 virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,
	                                                   const TSharedRef<STableViewBase>& OwnerTable) override;
	virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;
	
	// 新增：是否启用鼠标悬停选择
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "导航")
	bool bEnableHoverSelection = true;

	// 新增：当前选中索引
	int32 CurrentSelectedIndex = -1;
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameSettingVisualData> VisualData;
	
	// 新增：是否自动选择第一项
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "导航")
	bool bAutoSelectFirstItem = true;
	
	
private:
	TMap<FName, FText> NameOverrides;
};