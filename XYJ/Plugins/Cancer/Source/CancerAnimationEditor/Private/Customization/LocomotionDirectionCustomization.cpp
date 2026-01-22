
#include "Customization/LocomotionDirectionCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "Types/AnimInstanceDefinition.h"

/**
 * 自定义属性头部显示
 * @param PropertyHandle 要自定义的属性句柄
 * @param HeaderRow 属性头部的显示行
 * @param Utils 自定义工具实用程序
 */
void FLocomotionDirectionCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
                                                        FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& Utils)
{
    // 保存主属性句柄和工具实用程序
    OwnerPropertyHandle = PropertyHandle;
    AUtils = Utils.GetPropertyUtilities();
    
    
    // 获取模式属性句柄
    TSharedPtr<IPropertyHandle> Mode = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, Mode));
    
    // 创建强制刷新委托
    auto ForceRefreshDelegate = FSimpleDelegate::CreateSP(this, &FLocomotionDirectionCustomization::ForceRefresh);
    Mode->SetOnPropertyValueChanged(ForceRefreshDelegate);
    
    // 设置属性头部显示
    HeaderRow
    .NameContent()
    [
        PropertyHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    .MinDesiredWidth(400.f)  
    [
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .AutoWidth()  
        [
            Mode->CreatePropertyValueWidget()
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(STextBlock)
            .Text(this, &FLocomotionDirectionCustomization::GetModeText)
            .ColorAndOpacity(FLinearColor::Green)
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("刷新")))
            .ButtonStyle(FAppStyle::Get(), "SimpleButton")  
            .OnClicked(FOnClicked::CreateSP(this, &FLocomotionDirectionCustomization::OnRefreshClicked))
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(STextBlock)
            .Text(this, &FLocomotionDirectionCustomization::GetDataValidText)
            .Visibility(this, &FLocomotionDirectionCustomization::GetDataValidVisibility)
            .ColorAndOpacity(FLinearColor::Yellow)
        ]
    ];
}

/**
 * 获取当前模式的文本显示
 * @return 模式文本
 */
FText FLocomotionDirectionCustomization::GetModeText() const
{
    UE_LOG(LogTemp, Log, TEXT("GetModeText called"));
    
    if (OwnerPropertyHandle.IsValid())
    {
        uint8 ModeInt;
        if (OwnerPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, Mode))->GetValue(ModeInt))
        {
            ECancerDirAnim Mode = static_cast<ECancerDirAnim>(ModeInt);
            switch (Mode)
            {
            case ECancerDirAnim::Dir_4: return FText::FromString(TEXT("4方向"));
            case ECancerDirAnim::Dir_6: return FText::FromString(TEXT("6方向"));
            case ECancerDirAnim::Dir_8: return FText::FromString(TEXT("8方向"));
            default: return FText::FromString(TEXT("未知模式"));
            }
        }
    }
    
    return FText::FromString(TEXT("未初始化"));
}

/**
 * 自定义属性的子属性显示
 * @param PropertyHandle 主属性句柄
 * @param ChildBuilder 子属性构建器
 * @param Utils 自定义工具实用程序
 */
void FLocomotionDirectionCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
    IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& Utils)
{
    // 保存主属性句柄
    OwnerPropertyHandle = PropertyHandle;
    
    // 获取模式属性句柄
    TSharedPtr<IPropertyHandle> Mode = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, Mode));
    
    // 创建强制刷新委托
    auto ForceRefreshDelegate = FSimpleDelegate::CreateSP(this, &FLocomotionDirectionCustomization::ForceRefresh);
    Mode->SetOnPropertyValueChanged(ForceRefreshDelegate);
    
    // 添加新的子属性
    for (auto i : GetList(PropertyHandle))
    {
        ChildBuilder.AddProperty(i.ToSharedRef());
    }
}

/**
 * 获取属性列表
 * @param PropertyHandle 主属性句柄
 * @return 属性列表
 */
TArray<TSharedPtr<IPropertyHandle>> FLocomotionDirectionCustomization::GetList(
    TSharedRef<IPropertyHandle> PropertyHandle) const
{
    // 获取子属性句柄
    TSharedPtr<IPropertyHandle> F = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, F));
    TSharedPtr<IPropertyHandle> L = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, L));
    TSharedPtr<IPropertyHandle> R = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, R));
    TSharedPtr<IPropertyHandle> B = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, B));
    TSharedPtr<IPropertyHandle> FL = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, FL));
    TSharedPtr<IPropertyHandle> FR = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, FR));
    TSharedPtr<IPropertyHandle> BL = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, BL));
    TSharedPtr<IPropertyHandle> BR = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, BR));
    /*TSharedPtr<IPropertyHandle> LB = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCancerDirAnimation, LB));
    TSharedPtr<IPropertyHandle> RB = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCancerDirAnimation, RB));*/

    TArray<TSharedPtr<IPropertyHandle>> List;
    
    // 获取模式值
    ECancerDirAnim Mode;
    {
        uint8 ModeInt;
        PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocomotionDirection, Mode))->GetValue(ModeInt);
        Mode = static_cast<ECancerDirAnim>(ModeInt);
    }
    
    // 根据模式值设置属性列表
    switch (Mode)
    {
    case ECancerDirAnim::Dir_4: 
        List = {F,B,L,R};
        break;
    case ECancerDirAnim::Dir_6: 
        List = {F,B,FL,FR,BL,BR };
        break;
    case ECancerDirAnim::Dir_8: 
        List = {F,B,L,R,FL,FR,BL,BR};
        break;
    /*case ECancerDirAnim::Direction10:*/
    default:
        List = {F,B,L,R,FL,FR,BL,BR};
        break;
    }

    // 根据元数据隐藏属性
    const FString HideDirection = OwnerPropertyHandle->GetMetaData(TEXT("HideDirection"));
    if (!HideDirection.IsEmpty())
    {
        TArray<FString> HideList;
        HideDirection.ParseIntoArray(HideList, TEXT(","));
        for (auto NameStr : HideList)
        {
            const FName Name = FName(NameStr.TrimStartAndEnd());

            for (int i = List.Num() - 1; i >= 0; --i)
            {
                auto ph = List[i];
                if (ph && ph->GetProperty() && ph->GetProperty()->GetName() == Name)
                {
                    List.RemoveAt(i);
                }
            }
        }
    }
    return List;
}

/**
 * 强制刷新属性
 */
void FLocomotionDirectionCustomization::ForceRefresh()
{
    if (AUtils)
    {
        AUtils->ForceRefresh();
    }
}

/**
 * 刷新按钮点击处理
 * @return 按钮点击结果
 */
FReply FLocomotionDirectionCustomization::OnRefreshClicked()
{
    if (AUtils)
    {
        AUtils->ForceRefresh();
    }
    return FReply::Handled();
}

/**
 * 获取错误资产数量
 * @return 错误资产数量
 */
int32 FLocomotionDirectionCustomization::GetErrorAssetCount() const
{
    int32 Num = 0;
     
    // 判断资产是否为空
    auto HasAsset = [](TSharedPtr<IPropertyHandle> ph)
    {
        UObject* Asset = nullptr;
        ph->GetValue(Asset);
        return Asset != nullptr;
    };

    // 遍历属性列表
    for (auto ph : GetList(OwnerPropertyHandle.ToSharedRef()))
    {
        if (!HasAsset(ph))
        {
            Num++;
        }
    }

    return Num;
}

/**
 * 获取数据有效性文本
 * @return 数据有效性文本
 */
FText FLocomotionDirectionCustomization::GetDataValidText() const
{
    UE_LOG(LogTemp, Log, TEXT("GetDataValidText called"));
    
    return FText::FromString(FString::Printf(TEXT("缺失 %d 个动画资产"), GetErrorAssetCount()));
}

/**
 * 获取数据有效性可见性
 * @return 数据有效性可见性
 */
EVisibility FLocomotionDirectionCustomization::GetDataValidVisibility() const
{
    return GetErrorAssetCount() > 0 ? EVisibility::Visible : EVisibility::Hidden;
}
