#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Commands/Commands.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

class FToolBarBuilder;
class FMenuBuilder;

class FObjectDebuggerModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    /** 注册编辑器扩展 */
    void RegisterEditorExtensions();
    
    /** 注销编辑器扩展 */
    void UnregisterEditorExtensions();
    
    /** 创建工具栏按钮 */
    void AddToolbarExtension(FToolBarBuilder& Builder);
    
    /** 打开对象列表调试器 */
    void OpenObjectListDebugger();
    
    /** 编辑器扩展器 */
    TSharedPtr<FExtender> ToolbarExtender;
};

class FObjectDebuggerCommands : public TCommands<FObjectDebuggerCommands>
{
public:
    FObjectDebuggerCommands();
    
    /** 注册命令 */
    virtual void RegisterCommands() override;
    
    /** 打开对象列表调试器命令 */
    TSharedPtr<FUICommandInfo> OpenObjectListDebuggerCommand;
};
