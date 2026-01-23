#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "ObjectDebuggerCommands.generated.h"

class SObjectDebuggerOverlay;
class SObjectListDebuggerOverlay;

UCLASS()
class OBJECTDEBUGGER_API UObjectDebuggerCommands : public UObject
{
    GENERATED_BODY()

public:
    UObjectDebuggerCommands();
    
    virtual void BeginDestroy() override;
    
    /** 注册命令 */
    static void RegisterCommands();
    
    /** 注销命令 */
    static void UnregisterCommands();

    /** 打开一个新的对象列表调试器 */
    static void OpenNewObjectListDebugger();

private:
    
    /** 获取Actor的唯一标识 */
    static FName GetActorTabId(AActor* Actor);

private:
    static TArray<IConsoleObject*> RegisteredCommands;
    
    /** 当前的调试Overlay实例 */
    static TSharedPtr<SObjectDebuggerOverlay> DebugOverlayInstance;

    /** 是否显示调试信息 */
    static bool bIsShowDebugInfo;

    /** 对象列表调试器计数器，用于生成唯一ID */
    static int32 ObjectListDebuggerCount;
};
