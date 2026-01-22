#pragma once

#include "CoreMinimal.h"
#include "DebugConsoleSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDebugVarType : uint8
{
	Int,
	Float,
	String,
	BoolLike, // 以0/1表示的整型或浮点
};

USTRUCT(BlueprintType)
struct FDebugVarMeta
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Group;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDebugVarType Type = EDebugVarType::BoolLike;
};

USTRUCT(BlueprintType)
struct FDebugCommandMeta
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Group;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;
};

// 蓝图命令绑定：用于在运行时把控制台命令转发到某个对象的函数
USTRUCT()
struct FDebugCommandBPBinding
{
    GENERATED_BODY()
    // 目标对象（蓝图或C++均可）
    UPROPERTY()
    TObjectPtr<UObject> Target = nullptr;
    // 函数名：建议签名为
    // void Func(); 或 void Func(const FString& CommandLine);
    UPROPERTY()
    FName FunctionName = NAME_None;
};

DECLARE_DELEGATE_OneParam(FDebugCommandNative, const TArray<FString>&);
UCLASS()
class CANCERCONSOLE_API UDebugConsoleSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UDebugConsoleSubsystem* Get(const UObject* WorldContext);

    // —— 命令 —— //
    void RegisterCommand(const FDebugCommandMeta& Meta, FDebugCommandNative Callback);
    UFUNCTION(BlueprintCallable, Category="DebugConsole")
    void ExecuteCommand(const FString& CommandLine);
    // 蓝图绑定：注册命令并绑定到目标对象的函数（运行时可调用）
    UFUNCTION(BlueprintCallable, Category="DebugConsole")
    void RegisterCommandBinding(const FDebugCommandMeta& Meta, UObject* Target, FName FunctionName);
    // 取消命令注册并移除绑定
    UFUNCTION(BlueprintCallable, Category="DebugConsole")
    void UnregisterCommand(const FString& Name);

    // —— 变量 —— //
    void RegisterIntVar(const FDebugVarMeta& Meta, int32 DefaultValue, EConsoleVariableFlags Flags = ECVF_Default);
    void RegisterFloatVar(const FDebugVarMeta& Meta, float DefaultValue, EConsoleVariableFlags Flags = ECVF_Default);
    void RegisterStringVar(const FDebugVarMeta& Meta, const FString& DefaultValue,
                           EConsoleVariableFlags Flags = ECVF_Default);

	// 读/写（蓝图友好）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="DebugConsole")
	FString GetVarAsString(const FString& Name) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="DebugConsole")
	bool GetVarAsBool(const FString& Name) const;
	UFUNCTION(BlueprintCallable, Category="DebugConsole")
	void SetVarFromString(const FString& Name, const FString& InValue);

	// UI 数据源
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="DebugConsole")
	const TArray<FDebugVarMeta>& GetAllVarMetas() const { return VarMetas; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="DebugConsole")
    const TArray<FDebugCommandMeta>& GetAllCommandMetas() const { return CommandMetas; }

private:
    // 内部工具
    IConsoleVariable* FindVar(const FString& Name) const;
    void AddOrReplaceVarMeta(const FDebugVarMeta& Meta);
    void AddOrReplaceCmdMeta(const FDebugCommandMeta& Meta);
    void InvokeBPCommand(const FString& Name, const TArray<FString>& Args);

private:
    UPROPERTY()
    TArray<FDebugVarMeta> VarMetas;
    UPROPERTY()
    TArray<FDebugCommandMeta> CommandMetas;

    // 防止重复注册
    TSet<FString> RegisteredVars;
    TSet<FString> RegisteredCmds;

    // 蓝图命令绑定表
    UPROPERTY()
    TMap<FString, FDebugCommandBPBinding> BPBindings;
};
