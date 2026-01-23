#include "ObjectDebugger.h"
#include "ObjectDebuggerCommands.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/SlateTypes.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FObjectDebuggerModule"

// 命令类实现
FObjectDebuggerCommands::FObjectDebuggerCommands()
    : TCommands<FObjectDebuggerCommands>(
        TEXT("ObjectDebugger"), // 上下文名称
        NSLOCTEXT("Contexts", "ObjectDebugger", "对象调试器"), // 本地化上下文
        NAME_None, // 父上下文
        FAppStyle::GetAppStyleSetName() // 图标样式集
    )
{
}

void FObjectDebuggerCommands::RegisterCommands()
{
    UI_COMMAND(OpenObjectListDebuggerCommand, "对象列表调试器", "打开对象列表调试器", EUserInterfaceActionType::Button, FInputChord());
}

void FObjectDebuggerModule::StartupModule()
{
    // 注册所有控制台命令
    UObjectDebuggerCommands::RegisterCommands();
    
    // 注册编辑器扩展
    RegisterEditorExtensions();
}

void FObjectDebuggerModule::ShutdownModule()
{
    // 注销编辑器扩展
    UnregisterEditorExtensions();
    
    // 注销所有命令
    UObjectDebuggerCommands::UnregisterCommands();
}

void FObjectDebuggerModule::RegisterEditorExtensions()
{
    // 注册命令
    FObjectDebuggerCommands::Register();
    
    // 创建命令列表
    TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList);
    
    // 绑定命令到方法
    CommandList->MapAction(
        FObjectDebuggerCommands::Get().OpenObjectListDebuggerCommand,
        FExecuteAction::CreateRaw(this, &FObjectDebuggerModule::OpenObjectListDebugger),
        FCanExecuteAction());
    
    // 创建工具栏扩展
    ToolbarExtender = MakeShareable(new FExtender);
    ToolbarExtender->AddToolBarExtension(
        "EditorModes",
        EExtensionHook::After,
        CommandList,
        FToolBarExtensionDelegate::CreateRaw(this, &FObjectDebuggerModule::AddToolbarExtension)
    );
    
    // 获取编辑器模块
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FObjectDebuggerModule::UnregisterEditorExtensions()
{
    // 注销命令
    FObjectDebuggerCommands::Unregister();
    
    // 移除工具栏扩展
    if (ToolbarExtender.IsValid() && FModuleManager::Get().IsModuleLoaded("LevelEditor"))
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        LevelEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolbarExtender);
    }
}

void FObjectDebuggerModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(
        FObjectDebuggerCommands::Get().OpenObjectListDebuggerCommand,
        NAME_None,
        LOCTEXT("ObjectListDebuggerButton", "对象列表"),
        LOCTEXT("ObjectListDebuggerTooltip", "打开对象列表调试器"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Toolbar.Settings")
    );
}

void FObjectDebuggerModule::OpenObjectListDebugger()
{
    UObjectDebuggerCommands::OpenNewObjectListDebugger();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FObjectDebuggerModule, ObjectDebugger)
