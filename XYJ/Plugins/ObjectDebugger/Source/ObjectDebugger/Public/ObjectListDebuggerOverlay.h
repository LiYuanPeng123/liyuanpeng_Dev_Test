#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SSearchBox.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"

/** 类型选项结构体 */
struct FClassEntry
{
    /** 显示名称 */
    FString DisplayName;
    
    /** 实际类型 */
    TWeakObjectPtr<UClass> Class;
    
    /** 完整类型路径 */
    FString FullPath;
    
    FClassEntry(const FString& InDisplayName, UClass* InClass)
        : DisplayName(InDisplayName)
        , Class(InClass)
        , FullPath(InClass ? InClass->GetPathName() : TEXT("None"))
    {}

    bool MatchesSearchText(const FString& SearchText) const
    {
        if (SearchText.IsEmpty()) return true;
        
        return DisplayName.Contains(SearchText) || 
               FullPath.Contains(SearchText) ||
               (Class.IsValid() && Class->GetName().Contains(SearchText));
    }
};

/** 对象条目结构体 */
struct FObjectEntry
{
    /** 对象的弱引用 */
    TWeakObjectPtr<UObject> WeakObject;
    
    FObjectEntry(const TWeakObjectPtr<UObject>& InObject)
        : WeakObject(InObject)
    {}
};

class OBJECTDEBUGGER_API SObjectListDebuggerOverlay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SObjectListDebuggerOverlay)
    {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** 设置是否可见 */
    void SetOverlayVisible(bool bVisible);

private:
    /** 初始化所有可用的类型 */
    void InitializeClassOptions();

    /** 创建类型搜索框 */
    TSharedRef<SWidget> CreateClassSearchBox();
    
    /** 创建类型下拉框 */
    TSharedRef<SWidget> CreateClassComboBox();
    
    /** 创建对象列表 */
    TSharedRef<SWidget> CreateObjectList();

    /** 创建详情面板 */
    TSharedRef<SWidget> CreateDetailsView();
    
    /** 当搜索文本改变时 */
    void OnSearchTextChanged(const FText& InSearchText);
    
    /** 当选择的类型改变时 */
    void OnSelectedClassChanged(TSharedPtr<FClassEntry> NewSelection, ESelectInfo::Type SelectInfo);

    /** 当选择的对象改变时 */
    void OnSelectedObjectsChanged(TSharedPtr<FObjectEntry> Object, ESelectInfo::Type SelectInfo);
    
    /** 生成列表项 */
    TSharedRef<ITableRow> OnGenerateObjectRow(TSharedPtr<FObjectEntry> Object, const TSharedRef<STableViewBase>& OwnerTable);

    /** 过滤类型列表 */
    void FilterClassOptions();

    /** 下拉框打开事件 */
    void OnComboBoxOpened();

    /** 更新对象列表 */
    void UpdateObjectList();

    /** 定时刷新列表的回调 */
    EActiveTimerReturnType RefreshObjectList(double InCurrentTime, float InDeltaTime);

private:
    /** 自动刷新间隔 */
    static constexpr float RefreshInterval = 0.5f;

private:
    /** 当前选择的类型 */
    TSharedPtr<FClassEntry> CurrentClass;
    
    /** 所有可用的类型列表 */
    TArray<TSharedPtr<FClassEntry>> AllClassOptions;
    
    /** 过滤后的类型列表 */
    TArray<TSharedPtr<FClassEntry>> FilteredClassOptions;
    
    /** 当前的搜索文本 */
    FString CurrentSearchText;
    
    /** 类型选择下拉框 */
    TSharedPtr<SComboBox<TSharedPtr<FClassEntry>>> ClassComboBox;
    
    /** 对象列表视图 */
    TSharedPtr<SListView<TSharedPtr<FObjectEntry>>> ObjectListView;
    
    /** 当前显示的对象列表 */
    TArray<TSharedPtr<FObjectEntry>> CurrentObjects;

    /** 详情视图 */
    TSharedPtr<IDetailsView> DetailsView;
};
