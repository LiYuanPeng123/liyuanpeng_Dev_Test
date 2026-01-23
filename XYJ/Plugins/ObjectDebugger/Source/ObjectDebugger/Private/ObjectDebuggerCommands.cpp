#include "ObjectDebuggerCommands.h"
#include "Engine/World.h"
#include "Editor.h"
#include "Selection.h"
#include "Editor/EditorEngine.h"
#include "ObjectListDebuggerOverlay.h"
#include "Widgets/Docking/SDockTab.h"
#include "LevelEditor.h"

// 静态成员初始化
TArray<IConsoleObject*> UObjectDebuggerCommands::RegisteredCommands;
TSharedPtr<SObjectDebuggerOverlay> UObjectDebuggerCommands::DebugOverlayInstance = nullptr;
bool UObjectDebuggerCommands::bIsShowDebugInfo = false;
int32 UObjectDebuggerCommands::ObjectListDebuggerCount = 0;

FName UObjectDebuggerCommands::GetActorTabId(AActor* Actor)
{
    if (!Actor) return NAME_None;
    return FName(*FString::Printf(TEXT("ObjectDebugger_%s"), *Actor->GetName()));
}

void UObjectDebuggerCommands::RegisterCommands()
{
    RegisteredCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("OpenObjectListDebugger"),
        TEXT("打开一个新的对象列表调试器"),
        FConsoleCommandDelegate::CreateStatic(&UObjectDebuggerCommands::OpenNewObjectListDebugger)
    ));
}

void UObjectDebuggerCommands::OpenNewObjectListDebugger()
{
    // 生成唯一 TabId
    FName TabId = FName(*FString::Printf(TEXT("ObjectListDebugger_%d"), ObjectListDebuggerCount++));
    
    // 注册Tab生成器
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        TabId,
        FOnSpawnTab::CreateLambda([TabId](const FSpawnTabArgs& SpawnTabArgs)
        {
            return SNew(SDockTab)
                .TabRole(ETabRole::NomadTab)
                .Label(FText::FromString(TEXT("对象列表调试器")))
                .OnTabClosed_Lambda([TabId](TSharedRef<SDockTab> Tab)
                {
                    // 标签页关闭时，注销Tab生成器
                    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
                });
        }))
        .SetDisplayName(FText::FromString(TEXT("对象列表调试器")))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    // 创建Overlay实例
    TSharedPtr<SObjectListDebuggerOverlay> NewOverlay = SNew(SObjectListDebuggerOverlay);
    
    // 打开Tab并设置内容
    if (TSharedPtr<SDockTab> NewTab = FGlobalTabmanager::Get()->TryInvokeTab(TabId))
    {
        NewTab->SetContent(NewOverlay.ToSharedRef());
    }
}

UObjectDebuggerCommands::UObjectDebuggerCommands()
{
    // 构造函数中不需要特殊处理
}

void UObjectDebuggerCommands::BeginDestroy()
{
    UnregisterCommands();
    Super::BeginDestroy();
}

void UObjectDebuggerCommands::UnregisterCommands()
{
    // 注销已注册的命令
    for (IConsoleObject* Command : RegisteredCommands)
    {
        IConsoleManager::Get().UnregisterConsoleObject(Command);
    }
    RegisteredCommands.Empty();
    
    // 注销选择变化事件
#if WITH_EDITOR
    if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
        LevelEditorModule.OnActorSelectionChanged().RemoveAll(nullptr);
    }
#endif
}
