#include "DebugConsoleBindingBPLibrary.h"
#include "DebugConsoleSubsystem.h"

void UDebugConsoleBindingBPLibrary::BP_RegisterCommandBinding(const UObject* WCO, FDebugCommandMeta Meta, UObject* Target, FName FunctionName)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO))
    {
        S->RegisterCommandBinding(Meta, Target, FunctionName);
    }
}

void UDebugConsoleBindingBPLibrary::BP_UnregisterCommand(const UObject* WCO, const FString& Name)
{
    if (auto* S = UDebugConsoleSubsystem::Get(WCO))
    {
        S->UnregisterCommand(Name);
    }
}