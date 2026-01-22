
#include "DebugConsoleBPLibrary.h"
#include "DebugConsoleSubsystem.h"

void UDebugConsoleBPLibrary::BP_RegisterCommand(const UObject* WCO, FDebugCommandMeta Meta)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO))
    {
        // 蓝图端命令：转发为执行字符串（命令名本身即可执行）
        S->RegisterCommand(Meta, FDebugCommandNative::CreateLambda(
            [Cmd = Meta.Name](const TArray<FString>& Args)
            {
                UE_LOG(LogTemp, Log, TEXT("[BP Cmd] %s called, Args=%d"), *Cmd, Args.Num());
            }
        ));
    }
}

void UDebugConsoleBPLibrary::BP_ExecuteCommand(const UObject* WCO, const FString& CommandLine)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { S->ExecuteCommand(CommandLine); }
}

void UDebugConsoleBPLibrary::BP_RegisterIntVar(const UObject* WCO, FDebugVarMeta Meta, int32 DefaultValue)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { S->RegisterIntVar(Meta, DefaultValue, EConsoleVariableFlags::ECVF_Default); }
}

void UDebugConsoleBPLibrary::BP_RegisterFloatVar(const UObject* WCO, FDebugVarMeta Meta, float DefaultValue)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { S->RegisterFloatVar(Meta, DefaultValue, EConsoleVariableFlags::ECVF_Default); }
}

void UDebugConsoleBPLibrary::BP_RegisterStringVar(const UObject* WCO, FDebugVarMeta Meta, const FString& DefaultValue)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { S->RegisterStringVar(Meta, DefaultValue, EConsoleVariableFlags::ECVF_Default); }
}

FString UDebugConsoleBPLibrary::BP_GetVarAsString(const UObject* WCO, const FString& Name)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { return S->GetVarAsString(Name); }
    return TEXT("");
}

bool UDebugConsoleBPLibrary::BP_GetVarAsBool(const UObject* WCO, const FString& Name)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { return S->GetVarAsBool(Name); }
    return false;
}

void UDebugConsoleBPLibrary::BP_SetVarFromString(const UObject* WCO, const FString& Name, const FString& InValue)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO)) { S->SetVarFromString(Name, InValue); }
}
