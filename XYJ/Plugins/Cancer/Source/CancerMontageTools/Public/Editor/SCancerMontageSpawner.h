#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

struct FClassItem
{
    UClass* Class;
    FString Name;
};

class SCancerMontageSpawner : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SCancerMontageSpawner){}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedPtr<class SSearchBox> SearchBox;
    TArray<TSharedPtr<FClassItem>> ActorItems;
    TArray<TSharedPtr<FClassItem>> FilteredItems;
    TSharedPtr<SListView<TSharedPtr<FClassItem>>> ListView;

    void RefreshClassList();
    void ApplyFilter(const FString& Text);
    TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FClassItem> Item, const TSharedRef<STableViewBase>& OwnerTable) const;
    void OnSpawnClicked(TSharedPtr<FClassItem> Item) const;
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

};

