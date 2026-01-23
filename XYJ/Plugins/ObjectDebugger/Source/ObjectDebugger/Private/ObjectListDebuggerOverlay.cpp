#include "ObjectListDebuggerOverlay.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Editor.h"
#include "UObject/Class.h"
#include "UObject/UObjectIterator.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"

class FInlineObjectPropertyCustomization : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new FInlineObjectPropertyCustomization());
    }

    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override
    {
        HeaderRow
            .NameContent()
            [
                PropertyHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            [
                PropertyHandle->CreatePropertyValueWidget()
            ];
    }

    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override
    {
        // 检查递归深度
        // 我们只希望展开第一层对象属性
        // 如果当前属性的父级链中已经存在 ObjectProperty，说明我们已经是在嵌套展开中了，此时应该停止展开
        
        bool bIsNested = false;
        TSharedPtr<IPropertyHandle> ParentHandle = PropertyHandle->GetParentHandle();
        while (ParentHandle.IsValid())
        {
            if (ParentHandle->GetProperty() && ParentHandle->GetProperty()->IsA<FObjectProperty>())
            {
                bIsNested = true;
                break;
            }
            ParentHandle = ParentHandle->GetParentHandle();
        }

        if (bIsNested)
        {
            return;
        }

        UObject* Object = nullptr;
        PropertyHandle->GetValue(Object);
        
        if (Object)
        {
            // Add a header for the object class
            ChildBuilder.AddCustomRow(FText::FromString(TEXT("Class")))
                .NameContent()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("Instance Type")))
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                ]
                .ValueContent()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(Object->GetClass()->GetName()))
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                ];

            // Add all visible properties of the object as READ-ONLY TEXT
            // 这种方式避免了 AddExternalObjectProperty 带来的无限递归和性能崩溃风险
            for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
            {
                FProperty* Prop = *PropIt;
                
                // Show all UPROPERTY marked properties that are not deprecated
                if (!Prop->HasAnyPropertyFlags(CPF_Deprecated))
                {
                    FString ValueString;
                    
                    // 获取属性值的字符串表示
                    if (Prop->IsA<FObjectProperty>() || Prop->IsA<FInterfaceProperty>())
                    {
                         // 对象属性特殊处理，防止导出全部子对象文本导致卡死
                         // 我们只获取对象的名称
                         void* ValueAddress = Prop->ContainerPtrToValuePtr<void>(Object);
                         if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
                         {
                             UObject* ValueObj = ObjProp->GetObjectPropertyValue(ValueAddress);
                             if (ValueObj)
                             {
                                 ValueString = FString::Printf(TEXT("%s (%s)"), *ValueObj->GetName(), *ValueObj->GetClass()->GetName());
                             }
                             else
                             {
                                 ValueString = TEXT("None");
                             }
                         }
                         else
                         {
                             // Fallback for other types
                             Prop->ExportText_Direct(ValueString, ValueAddress, ValueAddress, nullptr, PPF_None);
                         }
                    }
                    else
                    {
                        void* ValueAddress = Prop->ContainerPtrToValuePtr<void>(Object);
                        Prop->ExportText_Direct(ValueString, ValueAddress, ValueAddress, nullptr, PPF_None);
                    }

                    // 添加只读文本行
                    ChildBuilder.AddCustomRow(FText::FromString(Prop->GetName()))
                        .NameContent()
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(Prop->GetDisplayNameText().ToString()))
                            .ToolTipText(FText::FromString(Prop->GetName()))
                            .Font(IDetailLayoutBuilder::GetDetailFont())
                        ]
                        .ValueContent()
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(ValueString))
                            .Font(IDetailLayoutBuilder::GetDetailFont())
                        ];
                }
            }
        }
    }
};

#include "Engine/BlueprintGeneratedClass.h"
#include "UObject/UnrealType.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SObjectListDebuggerOverlay::Construct(const FArguments& InArgs)
{
    // 初始化类型列表
    InitializeClassOptions();

    ChildSlot
    [
        SNew(SHorizontalBox)

        // 左侧：类型选择和对象列表
        + SHorizontalBox::Slot()
        .FillWidth(0.4f)
        [
            SNew(SVerticalBox)
            
            // 搜索框
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                CreateClassSearchBox()
            ]

            // 类型选择
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                CreateClassComboBox()
            ]

            // 对象列表
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            .Padding(5)
            [
                CreateObjectList()
            ]

            // 对象数量统计
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5, 2)
            .HAlign(HAlign_Right)
            [
                SNew(STextBlock)
                .Text_Lambda([this]()
                {
                    return FText::Format(FText::FromString(TEXT("Total: {0}")), FText::AsNumber(CurrentObjects.Num()));
                })
                .ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
            ]
        ]

        // 右侧：详情面板
        + SHorizontalBox::Slot()
        .FillWidth(0.6f)
        [
            CreateDetailsView()
        ]
    ];

    UpdateObjectList();

    // 注册定时刷新
    RegisterActiveTimer(RefreshInterval, FWidgetActiveTimerDelegate::CreateSP(this, &SObjectListDebuggerOverlay::RefreshObjectList));
}

TSharedRef<SWidget> SObjectListDebuggerOverlay::CreateDetailsView()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = true;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.bHideSelectionTip = true;
    DetailsViewArgs.bForceHiddenPropertyVisibility = true; // Show all properties
    
    DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    
    return DetailsView.ToSharedRef();
}

TSharedRef<SWidget> SObjectListDebuggerOverlay::CreateObjectList()
{
    return SAssignNew(ObjectListView, SListView<TSharedPtr<FObjectEntry>>)
        .ItemHeight(24)
        .ListItemsSource(&CurrentObjects)
        .OnGenerateRow(this, &SObjectListDebuggerOverlay::OnGenerateObjectRow)
        .OnSelectionChanged(this, &SObjectListDebuggerOverlay::OnSelectedObjectsChanged)
        .SelectionMode(ESelectionMode::Single);
}

void SObjectListDebuggerOverlay::OnSelectedObjectsChanged(TSharedPtr<FObjectEntry> Object, ESelectInfo::Type SelectInfo)
{
    if (DetailsView.IsValid() && Object.IsValid() && Object->WeakObject.IsValid())
    {
        UObject* SelectedObject = Object->WeakObject.Get();
        
        if (SelectedObject)
        {
            // Scan for object properties and register customization
            for (TFieldIterator<FProperty> PropIt(SelectedObject->GetClass()); PropIt; ++PropIt)
            {
                FProperty* Property = *PropIt;
                
                // Handle TArray<TObjectPtr<T>>
                if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
                {
                    if (FObjectProperty* InnerObjProp = CastField<FObjectProperty>(ArrayProp->Inner))
                    {
                        if (InnerObjProp->PropertyClass)
                        {
                            // Register for the class name
                            DetailsView->RegisterInstancedCustomPropertyTypeLayout(
                                InnerObjProp->PropertyClass->GetFName(),
                                FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInlineObjectPropertyCustomization::MakeInstance)
                            );
                        }
                    }
                }
                // Handle TObjectPtr<T>
                else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
                {
                    if (ObjProp->PropertyClass)
                    {
                        DetailsView->RegisterInstancedCustomPropertyTypeLayout(
                            ObjProp->PropertyClass->GetFName(),
                            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInlineObjectPropertyCustomization::MakeInstance)
                        );
                    }
                }
            }
        }
        
        DetailsView->SetObject(SelectedObject);
    }
}

void SObjectListDebuggerOverlay::InitializeClassOptions()
{
    AllClassOptions.Empty();
    
    // 遍历所有类
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* Class = *It;
        
        // 过滤掉已被新版本取代的类（热重载或蓝图编译产生的旧类）
        if (Class->HasAnyClassFlags(CLASS_NewerVersionExists))
        {
            continue;
        }

        FString ClassName = Class->GetName();

        // 过滤掉引擎生成的临时类、骨架类、垃圾回收类等
        if (ClassName.StartsWith(TEXT("SKEL_")) || 
            ClassName.StartsWith(TEXT("REINST_")) || 
            ClassName.StartsWith(TEXT("TRASH_")) ||
            ClassName.StartsWith(TEXT("HOTRELOADED_")) ||
            ClassName.StartsWith(TEXT("PLACEHOLDER_")))
        {
            continue;
        }

        // 优先使用类名作为显示名称
        FString DisplayName = ClassName;

        // 针对蓝图生成类（通常以 _C 结尾）的处理
        if (DisplayName.EndsWith(TEXT("_C")))
        {
            DisplayName.LeftChopInline(2);
        }
        else
        {
            // 对于 C++ 类，去除 U 或 A 前缀
            // 只有当第二个字母也是大写时才认为是前缀（例如 AActor -> Actor, 但 UObject -> Object, UberGraph -> UberGraph）
            if (DisplayName.Len() > 2 && (DisplayName.StartsWith(TEXT("U")) || DisplayName.StartsWith(TEXT("A"))))
            {
                if (FChar::IsUpper(DisplayName[1]))
                {
                    DisplayName.RightChopInline(1);
                }
            }
        }
        
        AllClassOptions.Add(MakeShareable(new FClassEntry(DisplayName, Class)));
    }

    // 按显示名称排序
    AllClassOptions.Sort([](const TSharedPtr<FClassEntry>& A, const TSharedPtr<FClassEntry>& B) {
        return A->DisplayName < B->DisplayName;
    });

    // 应用当前的搜索过滤
    FilterClassOptions();
}

TSharedRef<SWidget> SObjectListDebuggerOverlay::CreateClassSearchBox()
{
    return SNew(SSearchBox)
        .OnTextChanged(this, &SObjectListDebuggerOverlay::OnSearchTextChanged)
        .HintText(FText::FromString(TEXT("搜索类型...")));
}

TSharedRef<SWidget> SObjectListDebuggerOverlay::CreateClassComboBox()
{
    return SAssignNew(ClassComboBox, SComboBox<TSharedPtr<FClassEntry>>)
        .OptionsSource(&FilteredClassOptions)
        .OnSelectionChanged(this, &SObjectListDebuggerOverlay::OnSelectedClassChanged)
        .OnComboBoxOpening(this, &SObjectListDebuggerOverlay::OnComboBoxOpened)
        .OnGenerateWidget_Lambda([](TSharedPtr<FClassEntry> Item)
        {
            return SNew(STextBlock)
                .Text(FText::FromString(Item->DisplayName))
                .ToolTipText(FText::FromString(Item->FullPath));
        })
        [
            SNew(STextBlock)
            .Text_Lambda([this]()
            {
                return FText::FromString(CurrentClass.IsValid() ? CurrentClass->DisplayName : TEXT("选择类型"));
            })
        ];
}

void SObjectListDebuggerOverlay::OnSearchTextChanged(const FText& InSearchText)
{
    CurrentSearchText = InSearchText.ToString();
    FilterClassOptions();
}

void SObjectListDebuggerOverlay::FilterClassOptions()
{
    // 保存当前选中的类名，以便在列表刷新后尝试恢复选中状态
    FString CurrentSelectionName;
    if (CurrentClass.IsValid())
    {
        CurrentSelectionName = CurrentClass->DisplayName;
    }

    FilteredClassOptions.Empty();
    
    for (const auto& ClassOption : AllClassOptions)
    {
        if (ClassOption->MatchesSearchText(CurrentSearchText))
        {
            FilteredClassOptions.Add(ClassOption);
        }
    }

    // 尝试恢复选中项
    TSharedPtr<FClassEntry> NewSelection;
    if (!CurrentSelectionName.IsEmpty())
    {
        TSharedPtr<FClassEntry>* Found = FilteredClassOptions.FindByPredicate([&](const TSharedPtr<FClassEntry>& Item) {
            return Item->DisplayName == CurrentSelectionName;
        });
        
        if (Found)
        {
            NewSelection = *Found;
        }
    }

    // 如果没找到原来的，或者原来就没选中，则默认选第一个
    if (!NewSelection.IsValid() && FilteredClassOptions.Num() > 0)
    {
        NewSelection = FilteredClassOptions[0];
    }

    // 更新选中项
    if (CurrentClass != NewSelection)
    {
        CurrentClass = NewSelection;
        if (ClassComboBox.IsValid())
        {
            ClassComboBox->SetSelectedItem(CurrentClass);
        }
        UpdateObjectList();
    }

    // 刷新下拉框选项
    if (ClassComboBox.IsValid())
    {
        ClassComboBox->RefreshOptions();
    }
}

TSharedRef<ITableRow> SObjectListDebuggerOverlay::OnGenerateObjectRow(TSharedPtr<FObjectEntry> Object, const TSharedRef<STableViewBase>& OwnerTable)
{
    if (!Object.IsValid() || !Object->WeakObject.IsValid())
    {
        return SNew(STableRow<TSharedPtr<FObjectEntry>>, OwnerTable)
        [
            SNew(STextBlock).Text(FText::FromString(TEXT("无效对象")))
        ];
    }

    FString OwnerName = TEXT("None");
    UObject* Obj = Object->WeakObject.Get();
    if (AActor* Actor = Cast<AActor>(Obj))
    {
         if (Actor->GetOwner()) OwnerName = Actor->GetOwner()->GetName();
    }
    else if (UActorComponent* Component = Cast<UActorComponent>(Obj))
    {
         if (Component->GetOwner()) OwnerName = Component->GetOwner()->GetName();
    }
    else
    {
         if (Obj->GetOuter()) OwnerName = Obj->GetOuter()->GetName();
    }

    return SNew(STableRow<TSharedPtr<FObjectEntry>>, OwnerTable)
    [
        SNew(SVerticalBox)
        
        // 对象名称
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2)
        [
            SNew(STextBlock)
            .Text(FText::FromString(Object->WeakObject->GetName()))
        ]
        
        // 对象类型和路径
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2)
        [
            SNew(STextBlock)
            .Text(FText::FromString(Object->WeakObject->GetClass()->GetName() + TEXT(" (") + Object->WeakObject->GetPathName() + TEXT(")")))
            .ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f))
        ]

        // Owner/Outer 信息
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("Owner: ") + OwnerName))
            .ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
        ]
    ];
}

void SObjectListDebuggerOverlay::OnSelectedClassChanged(TSharedPtr<FClassEntry> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        CurrentClass = NewSelection;
        UpdateObjectList();
    }
}

void SObjectListDebuggerOverlay::OnComboBoxOpened()
{
    // 重新扫描所有类，以确保能找到新编译的蓝图类
    InitializeClassOptions();
}

void SObjectListDebuggerOverlay::UpdateObjectList()
{
    if (!GEditor || !CurrentClass.IsValid() || !CurrentClass->Class.IsValid())
    {
        CurrentObjects.Empty();
        if (ObjectListView.IsValid())
        {
            ObjectListView->RequestListRefresh();
        }
        return;
    }

    UClass* TargetClass = CurrentClass->Class.Get();
    if (!TargetClass)
    {
        CurrentObjects.Empty();
        if (ObjectListView.IsValid())
        {
            ObjectListView->RequestListRefresh();
        }
        return;
    }

    // 保存现有的条目以便复用（保持选中状态）
    TMap<TWeakObjectPtr<UObject>, TSharedPtr<FObjectEntry>> ExistingEntries;
    for (const auto& Entry : CurrentObjects)
    {
        if (Entry.IsValid() && Entry->WeakObject.IsValid())
        {
            ExistingEntries.Add(Entry->WeakObject, Entry);
        }
    }
    
    CurrentObjects.Empty();

    // 获取所有对象并进行安全检查
    for (TObjectIterator<UObject> It; It; ++It)
    {
        UObject* Object = *It;
        if (Object && !Object->IsUnreachable() && Object->IsA(TargetClass))
        {
            TWeakObjectPtr<UObject> WeakObject(Object);
            if (TSharedPtr<FObjectEntry>* FoundEntry = ExistingEntries.Find(WeakObject))
            {
                CurrentObjects.Add(*FoundEntry);
            }
            else
            {
                CurrentObjects.Add(MakeShareable(new FObjectEntry(WeakObject)));
            }
        }
    }

    // 按名称排序
    CurrentObjects.Sort([](const TSharedPtr<FObjectEntry>& A, const TSharedPtr<FObjectEntry>& B) {
        if (!A.IsValid() || !B.IsValid() || !A->WeakObject.IsValid() || !B->WeakObject.IsValid())
        {
            return false;
        }
        return A->WeakObject->GetName() < B->WeakObject->GetName();
    });

    if (ObjectListView.IsValid())
    {
        ObjectListView->RequestListRefresh();
    }
}

EActiveTimerReturnType SObjectListDebuggerOverlay::RefreshObjectList(double InCurrentTime, float InDeltaTime)
{
    // 只有在可见时才更新
    if (GetVisibility() == EVisibility::Visible)
    {
        UpdateObjectList();
    }
    return EActiveTimerReturnType::Continue;
}

void SObjectListDebuggerOverlay::SetOverlayVisible(bool bVisible)
{
    if (bVisible)
    {
        UpdateObjectList();
    }
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
