// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugConsoleSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugConsoleBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CANCERCONSOLE_API UDebugConsoleBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static void BP_RegisterCommand(const UObject* WorldContextObject, FDebugCommandMeta Meta);

	UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static void BP_ExecuteCommand(const UObject* WorldContextObject, const FString& CommandLine);
	
	UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static void BP_RegisterIntVar(const UObject* WorldContextObject, FDebugVarMeta Meta, int32 DefaultValue);
	UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static void BP_RegisterFloatVar(const UObject* WorldContextObject, FDebugVarMeta Meta, float DefaultValue);
	UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static void BP_RegisterStringVar(const UObject* WorldContextObject, FDebugVarMeta Meta, const FString& DefaultValue);

	UFUNCTION(BlueprintPure, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static FString BP_GetVarAsString(const UObject* WorldContextObject, const FString& Name);

	UFUNCTION(BlueprintPure, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static bool BP_GetVarAsBool(const UObject* WorldContextObject, const FString& Name);

	UFUNCTION(BlueprintCallable, Category="DebugConsole", meta=(WorldContext="WorldContextObject"))
	static void BP_SetVarFromString(const UObject* WorldContextObject, const FString& Name, const FString& InValue);
	
};
