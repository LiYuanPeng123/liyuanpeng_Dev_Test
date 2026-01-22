#include "EvaluatorType.h"

#include "Evaluator_Base.h"


DEFINE_LOG_CATEGORY(LogEvaluator);

FEvaluatorData::FEvaluatorData(): Evaluator(nullptr)
{
}

FEvaluatorData::FEvaluatorData(FString EvaluatorName)
{
	this->EvaluatorName = EvaluatorName;
}

double FEvaluator::Evaluator_Double(UObject* ContextObject)
{
	if (EvaluatorExpression.IsEmpty())
	{
		return 0;
	}
	return Evaluator(ContextObject);
}

bool FEvaluator::Evaluator_Bool(UObject* ContextObject)
{
	if (EvaluatorExpression.IsEmpty())
	{
		return true;
	}
	return static_cast<bool>(Evaluator(ContextObject));
}

void FEvaluator::PostSerialize(const FArchive& Ar)
{
	if (EvaluatorExpression.IsEmpty())
	{
		EvaluatorDataArray.Empty();
		ExpressionAnalysis.Empty();
	}
	else
	{
		//中缀转后缀表达式
		if (!ConvertInfixToSuffix(EvaluatorExpression, ExpressionAnalysisArray))
		{
			ConvertFailed();
			return;
		}
		if (ExpressionAnalysisArray.IsEmpty()) return;
		ExpressionAnalysis.Empty();
		for (FString ExpressionAnalysisStr : ExpressionAnalysisArray)
		{
			ExpressionAnalysis.Append(ExpressionAnalysisStr);
		}
		//排序
		TArray<FString> EvaluatorName;
		for (int32 x = 0; x < EvaluatorExpression.Len(); x++)
		{
			FString TestOperatorBlock;
			NextOperatorBlock(EvaluatorExpression, x, TestOperatorBlock, x);
			//不是数字和操作符
			if (!IsOperator(TestOperatorBlock) && !TestOperatorBlock.IsNumeric())
			{
				//按顺序记录出现的Key
				EvaluatorName.AddUnique(TestOperatorBlock);
			}
		}
		TArray<FEvaluatorData> NewEvaluatorDataArray;
		for (FString Name : EvaluatorName)
		{
			bool Find = false;
			for (FEvaluatorData& EvaluatorData : EvaluatorDataArray)
			{
				if (EvaluatorData.EvaluatorName == Name)
				{
					NewEvaluatorDataArray.Add(EvaluatorData);
					Find = true;
				}
			}
			if (!Find)
			{
				NewEvaluatorDataArray.Add(FEvaluatorData(Name));
			}
		}
		EvaluatorDataArray = NewEvaluatorDataArray;
		//测试求值
		double Result;
		if (EvaluatorResult(nullptr, Result))
		{
			SimulationResult_Bool = static_cast<bool>(Result);
			SimulationResult_Double = Result;
		}
		else
		{
			ConvertFailed();
		}
	}
}

double FEvaluator::Evaluator(UObject* ContextObject)
{
	if (!EvaluatorExpression.IsEmpty())
	{
		double Result;
		if (EvaluatorResult(ContextObject, Result))
		{
			return Result;
		}
	}
	EvaluatorFailed();
	return 0;
}

bool FEvaluator::EvaluatorResult(UObject* ContextObject, double& Result)
{
		//求值器获取值
	TMap<FString, double> EvaluatorMap;
	for (FEvaluatorData EvaluatorData : EvaluatorDataArray)
	{
		if (IsValid(EvaluatorData.Evaluator))
		{
			double Value = EvaluatorData.Evaluator->GetValue(ContextObject);
			EvaluatorMap.Add(EvaluatorData.EvaluatorName, Value);
		}
		else
		{
			UE_LOG(LogEvaluator, Warning, TEXT("求值器类为空%s"), *EvaluatorData.EvaluatorName)
		}
	}
	TArray<FString> Stack;
	int i = 0;
	while (i < ExpressionAnalysisArray.Num())
	{
		FString OperatorBlock = ExpressionAnalysisArray[i++];
		if (IsOperator(OperatorBlock))
		{
			if (OperatorBlock == "!")
			{
				double Operand;
				//参数
				FString Parameter;
				if (Stack.IsEmpty())
				{
					return false;
				}
				Parameter = Stack.Pop();
				//数值直接运算
				if (Parameter.IsNumeric())
				{
					Operand = FCString::Atod(*Parameter);
				}
				else
				{
					if (const double* Value = EvaluatorMap.Find(Parameter))
					{
						Operand = *Value;
					}
					else
					{
						Operand = 0;
					}
				}
				FString Value = FString::SanitizeFloat(!Operand);
				Stack.Add(Value);
			}
			else
			{
				double Operand1, Operand2;
				//参数2
				FString Parameter2;
				if (Stack.IsEmpty())
				{
					return false;
				}
				Parameter2 = Stack.Pop();
				if (Parameter2.IsNumeric())
				{
					Operand2 = FCString::Atod(*Parameter2);
				}
				else
				{
					if (const double* Value = EvaluatorMap.Find(Parameter2))
					{
						Operand2 = *Value;
					}
					else
					{
						Operand2 = 0;
					}
				}
				//参数1
				FString Parameter1;
				if (Stack.IsEmpty())
				{
					return false;
				}
				Parameter1 = Stack.Pop();
				if (Parameter1.IsNumeric())
				{
					Operand1 = FCString::Atod(*Parameter1);
				}
				else
				{
					if (const double* Value = EvaluatorMap.Find(Parameter1))
					{
						Operand1 = *Value;
					}
					else
					{
						Operand1 = 0;
					}
				}
				//开始计算
				if (OperatorBlock == "+")
				{
					FString Value = FString::SanitizeFloat(Operand1 + Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "-")
				{
					FString Value = FString::SanitizeFloat(Operand1 - Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "*")
				{
					FString Value = FString::SanitizeFloat(Operand1 * Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "/")
				{
					if (Operand2 == 0)
					{
						Stack.Add(FString(TEXT("0")));
					}
					else
					{
						FString Value = FString::SanitizeFloat(Operand1 / Operand2);
						Stack.Add(Value);
					}
				}
				else if (OperatorBlock == "^")
				{
					FString Value = FString::SanitizeFloat(FMath::Pow(Operand1, Operand2));
					Stack.Add(Value);
				}
				else if (OperatorBlock == "&&")
				{
					FString Value = FString::SanitizeFloat(Operand1 && Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "||")
				{
					FString Value = FString::SanitizeFloat(Operand1 || Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == ">")
				{
					FString Value = FString::SanitizeFloat(Operand1 > Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == ">=")
				{
					FString Value = FString::SanitizeFloat(Operand1 >= Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "<")
				{
					FString Value = FString::SanitizeFloat(Operand1 < Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "<=")
				{
					FString Value = FString::SanitizeFloat(Operand1 <= Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "==")
				{
					FString Value = FString::SanitizeFloat(Operand1 == Operand2);
					Stack.Add(Value);
				}
				else if (OperatorBlock == "!=")
				{
					FString Value = FString::SanitizeFloat(Operand1 != Operand2);
					Stack.Add(Value);
				}
			}
		}
		else
		{
			Stack.Add(OperatorBlock);
		}
	}
	if (Stack.Num() != 1)
	{
		return false;
	}
	//是数字
	if (Stack[0].IsNumeric())
	{
		Result = FCString::Atod(*Stack[0]);
		return true;
	}
	//不是操作符号
	if (!IsOperator(Stack[0]))
	{
		if (const double* ValuePtr = EvaluatorMap.Find(Stack[0]))
		{
			Result = *ValuePtr;
		}
		else
		{
			Result = 0;
		}
		return true;
	}
	return false;
}

bool FEvaluator::ConvertInfixToSuffix(FString InfixExpression, TArray<FString>& SuffixExpressionArray)
{
	if (InfixExpression.IsEmpty()) return false;
	SuffixExpressionArray.Empty();
	TArray<FString> OperatorStack;
	OperatorStack.Add(TEXT("#"));
	for (int32 i = 0; i < InfixExpression.Len(); i++)
	{
		//测试操作符
		FString TestCurOperator;
		NextOperatorBlock(InfixExpression, i, TestCurOperator, i);
		if (IsOperator(TestCurOperator))
		{
			if (TestCurOperator == "(")
			{
				OperatorStack.Add(TestCurOperator);
				continue;
			}
			if (TestCurOperator == ")")
			{
				while (OperatorStack.Last() != "#" && OperatorStack.Last() != "(")
				{
					SuffixExpressionArray.Add(OperatorStack.Pop());
				}
				if (OperatorStack.Last() == "#")
				{
					return false;
				}
				OperatorStack.Pop();
				continue;
			}
			while (OperatorStack.Last() != "#" &&
				OperatorPrecedence(OperatorStack.Last()) > 1 &&
				OperatorPrecedence(OperatorStack.Last()) <= OperatorPrecedence(TestCurOperator))
			{
				SuffixExpressionArray.Add(OperatorStack.Pop());
			}
			OperatorStack.Add(TestCurOperator);
		}
		else
		{
			SuffixExpressionArray.Add(TestCurOperator);
		}
	}
	while (OperatorStack.Last() != "#")
	{
		SuffixExpressionArray.Add(OperatorStack.Pop());
	}
	return true;
}

int32 FEvaluator::OperatorPrecedence(FString Symbol)
{
	if (Symbol == TEXT("(") ||
	Symbol == TEXT(")") ||
	Symbol == TEXT("#"))
	{
		return 1;
	}
	if (Symbol == TEXT("!"))
	{
		return 2;
	}
	if (Symbol == TEXT("^"))
	{
		return 3;
	}
	if (Symbol == TEXT("*") ||
		Symbol == TEXT("/"))
	{
		return 4;
	}
	if (Symbol == TEXT("+") ||
		Symbol == TEXT("-"))
	{
		return 5;
	}
	if (Symbol == TEXT(">") ||
		Symbol == TEXT(">=") ||
		Symbol == TEXT("<") ||
		Symbol == TEXT("<=") ||
		Symbol == TEXT("!=") ||
		Symbol == TEXT("=="))
	{
		return 6;
	}
	if (Symbol == TEXT("&&") ||
		Symbol == TEXT("||"))
	{
		return 7;
	}
	return 0;
}

bool FEvaluator::IsOperator(FString Symbol)
{
	if (Symbol == TEXT("+") ||
		Symbol == TEXT("-") ||
		Symbol == TEXT("*") ||
		Symbol == TEXT("/") ||
		Symbol == TEXT("^") ||
		Symbol == TEXT("(") ||
		Symbol == TEXT(")") ||
		Symbol == TEXT("&&") ||
		Symbol == TEXT("||") ||
		Symbol == TEXT(">") ||
		Symbol == TEXT(">=") ||
		Symbol == TEXT("<") ||
		Symbol == TEXT("<=") ||
		Symbol == TEXT("==") ||
		Symbol == TEXT("!=") ||
		Symbol == TEXT("!"))
	{
		return true;
	}
	return false;
}

bool FEvaluator::ContainOperator(FString Symbol)
{
	TArray<FString> OperatorArray;
	OperatorArray.Add(TEXT("+"));
	OperatorArray.Add(TEXT("-"));
	OperatorArray.Add(TEXT("*"));
	OperatorArray.Add(TEXT("/"));
	OperatorArray.Add(TEXT("^"));
	OperatorArray.Add(TEXT("("));
	OperatorArray.Add(TEXT(")"));
	OperatorArray.Add(TEXT("&&"));
	OperatorArray.Add(TEXT("||"));
	OperatorArray.Add(TEXT(">"));
	OperatorArray.Add(TEXT(">="));
	OperatorArray.Add(TEXT("<"));
	OperatorArray.Add(TEXT("<="));
	OperatorArray.Add(TEXT("=="));
	OperatorArray.Add(TEXT("!="));
	OperatorArray.Add(TEXT("!"));
	TArray<FString> OperatorFilter;
	if (Symbol.Len() > 0 && Symbol[0])
	{
		OperatorFilter = OperatorArray;
	}
	for (int i = 0; i < Symbol.Len(); ++i)
	{
		TCHAR Char = Symbol[i];
		TArray<FString> NewOperatorFilter;
		for (FString Operator : OperatorFilter)
		{
			if (i < Operator.Len())
			{
				if (Char == Operator[i])
				{
					NewOperatorFilter.Add(Operator);
				}
			}
		}
		OperatorFilter = NewOperatorFilter;
	}
	return OperatorFilter.Num() > 0;
}

void FEvaluator::ConvertFailed()
{
	ExpressionAnalysis = TEXT("表达式解析错误");
	UE_LOG(LogEvaluator, Warning, TEXT("表达式解析错误%s"), *EvaluatorExpression)
}

void FEvaluator::EvaluatorFailed()
{
	UE_LOG(LogEvaluator, Warning, TEXT("计算时出错%s"), *EvaluatorExpression)
}

void FEvaluator::NextOperatorBlock(FString Expression, int32 FindIndex, FString& OperatorBlock, int32& EndIndex)
{
	if (FindIndex >= Expression.Len()) return;
	OperatorBlock.Empty();
	OperatorBlock.AppendChar(Expression[FindIndex]);
	bool IsOperatorBlock = ContainOperator(OperatorBlock);
	EndIndex = FindIndex;
	bool IsBreak = false;
	while (EndIndex < Expression.Len() || !IsBreak)
	{
		int32 TempIndex = EndIndex + 1;
		if (TempIndex >= Expression.Len())
		{
			IsBreak = true;
			break;
		}
		FString TempNextOperator = OperatorBlock;
		TCHAR NextSymbol = Expression[TempIndex];
		TempNextOperator.AppendChar(NextSymbol);
		FString TempSymbol;
		TempSymbol.AppendChar(NextSymbol);
		if ((IsOperatorBlock && ContainOperator(TempNextOperator) && ContainOperator(TempSymbol)) ||
			(!IsOperatorBlock && !ContainOperator(TempNextOperator) && !ContainOperator(TempSymbol)))
		{
			OperatorBlock = TempNextOperator;
			EndIndex = TempIndex;
		}
		else
		{
			IsBreak = true;
			break;
		}
	}
}
