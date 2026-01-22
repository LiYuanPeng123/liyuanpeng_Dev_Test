#include "CancerMontageTools/Public/Editor/SCancerMontageSpawner.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/UObjectIterator.h"
#include "CancerMontageTools/Public/Editor/ProjectilePreviewAPI.h"
#include "EditorModeManager.h"
#include "Editor.h"

#include "Editor.h"

static UWorld* FindPreviewWorld()
{
    if (!GEngine) return nullptr;
    for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
    {
        UWorld* World = Ctx.World();
        if (World && World->IsPreviewWorld())
        {
            return World;
        }
    }
    return nullptr;
}

static void SetPreviewViewportWidgetMode(UE::Widget::EWidgetMode Mode)
{
    if (!GEditor) return;
    const TArray<FEditorViewportClient*>& Clients = GEditor->GetAllViewportClients();
    for (FEditorViewportClient* C : Clients)
    {
        if (!C) continue;
        UWorld* W = C->GetWorld();
        if (W && W->IsPreviewWorld())
        {
            C->SetWidgetMode(Mode);
        }
    }
}

void SCancerMontageSpawner::Construct(const FArguments& InArgs)
{
    RefreshClassList();
    FilteredItems = ActorItems;

    ChildSlot
    [
        SNew(SBorder)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(4)
            [
                SAssignNew(SearchBox, SSearchBox)
                .OnTextChanged_Lambda([this](const FText& Text){ ApplyFilter(Text.ToString()); })
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(4)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().Padding(2,0)
                [
                    SNew(SButton)
                    .Text(FText::FromString(TEXT("选择模式")))
                    .OnClicked_Lambda([](){ SetPreviewViewportWidgetMode(UE::Widget::WM_None); return FReply::Handled(); })
                ]
                + SHorizontalBox::Slot().AutoWidth().Padding(2,0)
                [
                    SNew(SButton)
                    .Text(FText::FromString(TEXT("平移模式")))
                    .OnClicked_Lambda([](){ SetPreviewViewportWidgetMode(UE::Widget::WM_Translate); return FReply::Handled(); })
                ]
                + SHorizontalBox::Slot().AutoWidth().Padding(2,0)
                [
                    SNew(SButton)
                    .Text(FText::FromString(TEXT("旋转模式")))
                    .OnClicked_Lambda([](){ SetPreviewViewportWidgetMode(UE::Widget::WM_Rotate); return FReply::Handled(); })
                ]
                + SHorizontalBox::Slot().AutoWidth().Padding(2,0)
                [
                    SNew(SButton)
                    .Text(FText::FromString(TEXT("缩放模式")))
                    .OnClicked_Lambda([](){ SetPreviewViewportWidgetMode(UE::Widget::WM_Scale); return FReply::Handled(); })
                ]
            ]
            + SVerticalBox::Slot().FillHeight(1.f).Padding(4)
            [
                SAssignNew(ListView, SListView<TSharedPtr<FClassItem>>)
                .ListItemsSource(&FilteredItems)
                .OnGenerateRow(this, &SCancerMontageSpawner::OnGenerateRow)
                .SelectionMode(ESelectionMode::Single)
            ]
        ]
    ];
}

void SCancerMontageSpawner::RefreshClassList()
{
    ActorItems.Empty();
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* Cls = *It;
        if (!Cls) continue;
        if (!Cls->IsChildOf(AActor::StaticClass())) continue;
        if (Cls->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)) continue;
        TSharedPtr<FClassItem> Item = MakeShared<FClassItem>();
        Item->Class = Cls;
        Item->Name = Cls->GetName();
        ActorItems.Add(Item);
    }
}

void SCancerMontageSpawner::ApplyFilter(const FString& Text)
{
    FilteredItems.Empty();
    const FString Query = Text.ToLower();
    for (const TSharedPtr<FClassItem>& Item : ActorItems)
    {
        const FString Name = Item.IsValid() ? Item->Name.ToLower() : FString();
        if (Query.IsEmpty() || Name.Contains(Query))
        {
            FilteredItems.Add(Item);
        }
    }
    if (ListView)
    {
        ListView->RequestListRefresh();
    }
}

TSharedRef<ITableRow> SCancerMontageSpawner::OnGenerateRow(TSharedPtr<FClassItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const
{
    UClass* Cls = Item.IsValid() ? Item->Class : nullptr;
    const FString Label = Item.IsValid() ? Item->Name : TEXT("<None>");
    return SNew(STableRow<TSharedPtr<UClass>>, OwnerTable)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
        [
            SNew(STextBlock).Text(FText::FromString(Label))
        ]
        + SHorizontalBox::Slot().AutoWidth().Padding(4,0)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("生成到预览")))
            .OnClicked_Lambda([Item, this]()
            {
                OnSpawnClicked(Item);
                return FReply::Handled();
            })
        ]
        + SHorizontalBox::Slot().AutoWidth().Padding(4,0)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("拖拽到视口放置")))
            .OnClicked_Lambda([Item]()
            {
                if (Item.IsValid() && Item->Class)
                {
                    StartCancerDragPlace(Item->Class);
                }
                return FReply::Handled();
            })
        ]
    ];
}

void SCancerMontageSpawner::OnSpawnClicked(TSharedPtr<FClassItem> Item) const
{
    UWorld* PreviewWorld = FindPreviewWorld();
    if (!PreviewWorld || !Item.IsValid() || !Item->Class) return;
    const FVector DefaultPos(100.f, 0.f, 100.f);
    const FRotator DefaultRot = FRotator::ZeroRotator;
    PreviewWorld->SpawnActor<AActor>(Item->Class, DefaultPos, DefaultRot);
}
