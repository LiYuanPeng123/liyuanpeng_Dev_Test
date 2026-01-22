#include "DebugConsoleSubsystem.h"


UDebugConsoleSubsystem* UDebugConsoleSubsystem::Get(const UObject* WorldContext)
{
    if (!WorldContext) return nullptr;
    if (const UGameInstance* GI = WorldContext->GetWorld() ? WorldContext->GetWorld()->GetGameInstance() : nullptr)
    {
        return GI->GetSubsystem<UDebugConsoleSubsystem>();
    }
    return nullptr;
}

void UDebugConsoleSubsystem::RegisterCommand(const FDebugCommandMeta& Meta, FDebugCommandNative Callback)
{
    if (RegisteredCmds.Contains(Meta.Name)) { AddOrReplaceCmdMeta(Meta); return; }

    IConsoleManager::Get().RegisterConsoleCommand(
        *Meta.Name, *Meta.Description,
        FConsoleCommandWithArgsDelegate::CreateLambda([Callback](const TArray<FString>& Args)
        {
            Callback.ExecuteIfBound(Args);
        }),
        ECVF_Default
    );
    RegisteredCmds.Add(Meta.Name);
    AddOrReplaceCmdMeta(Meta);
}

void UDebugConsoleSubsystem::ExecuteCommand(const FString& CommandLine)
{
    if (GEngine && GEngine->GameViewport)
    {
        UWorld* World = GEngine->GameViewport->GetWorld();
        if (World) { GEngine->Exec(World, *CommandLine); }
    }
}

void UDebugConsoleSubsystem::RegisterIntVar(const FDebugVarMeta& Meta, int32 DefaultValue, EConsoleVariableFlags Flags)
{
    if (!RegisteredVars.Contains(Meta.Name))
    {
        IConsoleManager::Get().RegisterConsoleVariable(*Meta.Name, DefaultValue, *Meta.Description, Flags);
        RegisteredVars.Add(Meta.Name);
    }
    AddOrReplaceVarMeta(Meta);
}

void UDebugConsoleSubsystem::RegisterFloatVar(const FDebugVarMeta& Meta, float DefaultValue, EConsoleVariableFlags Flags)
{
    if (!RegisteredVars.Contains(Meta.Name))
    {
        IConsoleManager::Get().RegisterConsoleVariable(*Meta.Name, DefaultValue, *Meta.Description, Flags);
        RegisteredVars.Add(Meta.Name);
    }
    AddOrReplaceVarMeta(Meta);
}

void UDebugConsoleSubsystem::RegisterStringVar(const FDebugVarMeta& Meta, const FString& DefaultValue, EConsoleVariableFlags Flags)
{
    if (!RegisteredVars.Contains(Meta.Name))
    {
        IConsoleManager::Get().RegisterConsoleVariable(*Meta.Name, *DefaultValue, *Meta.Description, Flags);
        RegisteredVars.Add(Meta.Name);
    }
    AddOrReplaceVarMeta(Meta);
}

IConsoleVariable* UDebugConsoleSubsystem::FindVar(const FString& Name) const
{
    return IConsoleManager::Get().FindConsoleVariable(*Name);
}

FString UDebugConsoleSubsystem::GetVarAsString(const FString& Name) const
{
    if (IConsoleVariable* Var = FindVar(Name))
    {
        return Var->GetString();
    }
    return TEXT("");
}

bool UDebugConsoleSubsystem::GetVarAsBool(const FString& Name) const
{
    if (IConsoleVariable* Var = FindVar(Name))
    {
        return FCString::Atoi(*Var->GetString()) != 0;
    }
    return false;
}

void UDebugConsoleSubsystem::SetVarFromString(const FString& Name, const FString& InValue)
{
    if (IConsoleVariable* Var = FindVar(Name))
    {
        Var->Set(*InValue);
    }
}

void UDebugConsoleSubsystem::AddOrReplaceVarMeta(const FDebugVarMeta& Meta)
{
    for (auto& M : VarMetas)
    {
        if (M.Name == Meta.Name) { M = Meta; return; }
    }
    VarMetas.Add(Meta);
}

void UDebugConsoleSubsystem::AddOrReplaceCmdMeta(const FDebugCommandMeta& Meta)
{
    for (auto& M : CommandMetas)
    {
        if (M.Name == Meta.Name) { M = Meta; return; }
    }
    CommandMetas.Add(Meta);
}