#include "DebugConsoleSubsystem.h"

void UDebugConsoleSubsystem::RegisterCommandBinding(const FDebugCommandMeta& Meta, UObject* Target, FName FunctionName)
{
    if (!Target || FunctionName.IsNone()) { return; }
    // 通过原有注册接口注册命令，同时绑定蓝图处理逻辑
    TWeakObjectPtr<UObject> WeakTarget(Target);
    const FName Func = FunctionName;
    RegisterCommand(Meta, FDebugCommandNative::CreateLambda([WeakTarget, Func](const TArray<FString>& Args)
    {
        UObject* T = WeakTarget.Get();
        if (!T) { return; }
        UFunction* F = T->FindFunction(Func);
        if (!F) { return; }
        // 支持两种签名：void Func(); 或 void Func(const FString& CommandLine)
        if (F->NumParms == 0)
        {
            T->ProcessEvent(F, nullptr);
        }
        else if (F->NumParms == 1)
        {
            for (TFieldIterator<FProperty> It(F); It; ++It)
            {
                if (It->IsA(FStrProperty::StaticClass()))
                {
                    FString ArgLine = FString::Join(Args, TEXT(" "));
                    struct FParam { FString Value; } Param{ ArgLine };
                    T->ProcessEvent(F, &Param);
                    break;
                }
            }
        }
    }));
}

void UDebugConsoleSubsystem::UnregisterCommand(const FString& Name)
{
    if (!RegisteredCmds.Contains(Name)) { return; }
    IConsoleManager::Get().UnregisterConsoleObject(*Name, /*bShouldDeleteObject*/ false);
    RegisteredCmds.Remove(Name);
    for (int32 i = 0; i < CommandMetas.Num(); ++i)
    {
        if (CommandMetas[i].Name == Name)
        {
            CommandMetas.RemoveAt(i);
            break;
        }
    }
}

void UDebugConsoleSubsystem::InvokeBPCommand(const FString& Name, const TArray<FString>& Args)
{
    // 在当前实现中，蓝图命令通过 RegisterCommandBinding 的 lambda 执行；本函数留空以便后续扩展。
}