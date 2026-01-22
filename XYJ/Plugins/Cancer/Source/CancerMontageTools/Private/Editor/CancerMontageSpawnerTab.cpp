#include "CancerMontageTools/Public/Editor/SCancerMontageSpawner.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"

static const FName CancerMontageSpawnerTabId(TEXT("CancerMontageSpawner"));

static TSharedRef<SDockTab> SpawnCancerMontageSpawnerTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SCancerMontageSpawner)
        ];
}

struct FCancerAutoRegisterMontageTab
{
    FCancerAutoRegisterMontageTab()
    {
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CancerMontageSpawnerTabId, FOnSpawnTab::CreateStatic(&SpawnCancerMontageSpawnerTab))
        .SetDisplayName(FText::FromString(TEXT("蒙太奇生成器")))
        .SetTooltipText(FText::FromString(TEXT("搜索并生成预览Actor")));
    }
    ~FCancerAutoRegisterMontageTab()
    {
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CancerMontageSpawnerTabId);
    }
};

static FCancerAutoRegisterMontageTab GCancerAutoRegisterMontageTabInstance;

