#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugConsoleSubsystem.h"
#include "DebugConsoleBindingBPLibrary.generated.h"

UCLASS()
class CANCERCONSOLE_API UDebugConsoleBindingBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    // 支持签名：void Func(); 或 void Func(const FString& CommandLine)
    UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
    static void BP_RegisterCommandBinding(const UObject* WorldContextObject, FDebugCommandMeta Meta, UObject* Target, FName FunctionName);

    // 取消命令注册
    UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
    static void BP_UnregisterCommand(const UObject* WorldContextObject, const FString& Name);
};