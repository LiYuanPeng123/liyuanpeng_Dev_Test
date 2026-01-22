#pragma once

#include "CoreMinimal.h"
#include "EvaluatorType.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEvaluator, Warning, All);

UENUM(BlueprintType)
enum class EEvaluatorType : uint8
{
	Bool UMETA(DisplayName = "逻辑运算"),
	Double UMETA(DisplayName = "数值运算"),
};


USTRUCT(BlueprintType)
struct CANCEREVALUATOR_API FEvaluatorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, DisplayName="求值器名称")
	FString EvaluatorName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="求值描述")
	FString EvaluatorDesc;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, DisplayName="求值器")
	TObjectPtr<class UEvaluator_Base> Evaluator;

	FEvaluatorData();
	FEvaluatorData(FString EvaluatorName);
};


USTRUCT(BlueprintType)
struct CANCEREVALUATOR_API FEvaluator
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="求值类型")
	EEvaluatorType EvaluatorType = EEvaluatorType::Bool;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="求值表达式",
		meta=(ToolTip=
			"表达式示例:(参数1+参数2)*参数3\n支持的运算符:(),&&,||,>,>=,<,<=,==,!=,!,+,-,*,/,^\n求值类型仅代表返回值类型,支持整数和小数解析"
		))
	FString EvaluatorExpression;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, DisplayName="表达式解析")
	FString ExpressionAnalysis;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, DisplayName="模拟结果",
		meta=(EditConditionHides, EditCondition="EvaluatorType==EEvaluatorType::Double"))
	double SimulationResult_Double = 0.f;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, DisplayName="模拟结果",
		meta=(EditConditionHides, EditCondition="EvaluatorType==EEvaluatorType::Bool"))
	bool SimulationResult_Bool = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="表达式描述")
	FString ExpressionDesc;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="求值器列表", EditFixedSize,
		meta=(TitleProperty="EvaluatorName"))
	TArray<FEvaluatorData> EvaluatorDataArray;

	double Evaluator_Double(UObject* ContextObject);
	bool Evaluator_Bool(UObject* ContextObject);

	void PostSerialize(const FArchive& Ar);

private:
	//求值
	double Evaluator(UObject* ContextObject);
	bool EvaluatorResult(UObject* ContextObject, double& Result);
	//中缀表达式转后缀表达式
	bool ConvertInfixToSuffix(FString InfixExpression, TArray<FString>& SuffixExpressionArray);
	//操作符优先级
	int32 OperatorPrecedence(FString Symbol);
	//是否是操作符
	bool IsOperator(FString Symbol);
	//包含操作符
	bool ContainOperator(FString Symbol);
	//转换失败
	void ConvertFailed();
	//计算失败
	void EvaluatorFailed();
	//获取下一个操作块
	void NextOperatorBlock(FString Expression, int32 FindIndex, FString& OperatorBlock, int32& EndIndex);

	TArray<FString> ExpressionAnalysisArray;
};

template <>
struct TStructOpsTypeTraits<FEvaluator> : public TStructOpsTypeTraitsBase2<FEvaluator>
{
	enum
	{
		// WithZeroConstructor = false, // 0构造，内存清零后就可以了，说明这个结构的默认值就是0
		// WithNoInitConstructor = false, // 有个ForceInit的参数的构造，用来专门构造出0值结构来
		WithNoDestructor = true, // 是否没有结构有自定义的析构函数， 如果没有析构的话，DestroyStruct里面就可以省略调用析构函数了。默认是有的。结构如果是pod类型，则肯定没有析构。
		// WithCopy = false, // 是否结构有自定义的=赋值函数。如果没有的话，在CopyScriptStruct的时候就只需要拷贝内存就可以了
		// WithIdenticalViaEquality = false, // 用==来比较结构
		// WithIdentical = false, // 有一个自定义的Identical函数来专门用来比较，和WithIdenticalViaEquality互斥
		// WithExportTextItem = false, // 有一个ExportTextItem函数来把结构值导出为字符串
		// WithImportTextItem = false, // 有一个ImportTextItem函数把字符串导进结构值
		// WithAddStructReferencedObjects = false, // 有一个AddStructReferencedObjects函数用来添加结构额外的引用对象
		// WithSerializer = false, // 有一个Serialize函数用来序列化
		// WithStructuredSerializer = false, // 有一个结构结构Serialize函数用来序列化
		WithPostSerialize = true, // 有一个PostSerialize回调用来在序列化后调用
		// WithNetSerializer = false, // 有一个NetSerialize函数用来在网络复制中序列化
		// WithNetDeltaSerializer = false, // 有一个NetDeltaSerialize函数用来在之前NetSerialize的基础上只序列化出差异来，一般用在TArray属性上进行优化
		// WithSerializeFromMismatchedTag = false, // 有一个SerializeFromMismatchedTag函数用来处理属性tag未匹配到的属性值，一般是在结构进行升级后，但值还是原来的值，这个时候用来把旧值升级到新结构时使用
		// WithStructuredSerializeFromMismatchedTag = false, // SerializeFromMismatchedTag的结构版本
		// WithPostScriptConstruct = false, // 有一个PostScriptConstruct函数用在蓝图构造脚本后调用
		// WithNetSharedSerialization = false, // 指明结构的NetSerialize函数不需要用到UPackageMap
	};
};
