#include "Chosse/CancerChooserLibrary.h"
#include "ChooserFunctionLibrary.h"
#include "IObjectChooser.h"
#include "InstancedStruct.h"

UAnimSequenceBase* UCancerChooserLibrary::Generic_GetAnimFromChooser(AActor* TargetActor, const FInstancedStruct& ChooserData, UChooserTable* ChooserTable, bool& bIsValid, TSubclassOf<UAnimSequenceBase> OutputClass)
{
	bIsValid = false;

	if (!ChooserTable)
	{
		return nullptr;
	}

	FChooserEvaluationContext EvalCtx = UChooserFunctionLibrary::MakeChooserEvaluationContext();
    
	// Add the struct data
	// Note: EvalCtx.Params is TArray<FStructView>. FStructView requires mutable memory.
	// Since ChooserData is const, we need to make a copy if we want to create a valid FStructView,
	// or use AddStructParam if available. But AddStructParam is a template.
	// We manually add it as a view.
	FInstancedStruct MutableData = ChooserData;
	EvalCtx.Params.Add(FStructView(MutableData.GetScriptStruct(), MutableData.GetMutableMemory()));

	// Add the Actor
	if (TargetActor)
	{
		EvalCtx.AddObjectParam(TargetActor);
	}

	FInstancedStruct EvaluateStruct = UChooserFunctionLibrary::MakeEvaluateChooser(ChooserTable);

	UClass* ResultClass = OutputClass ? OutputClass.Get() : UAnimSequenceBase::StaticClass();

	UObject* ResultObj = UChooserFunctionLibrary::EvaluateObjectChooserBase(
		EvalCtx,
		EvaluateStruct,
		ResultClass,
		/*bResultIsClass*/ false
	);

	if (ResultObj)
	{
		bIsValid = true;
		return Cast<UAnimSequenceBase>(ResultObj);
	}
    
	return nullptr;
}

DEFINE_FUNCTION(UCancerChooserLibrary::execGetAnimFromChooserBP)
{
	P_GET_OBJECT(AActor, TargetActor);
    
	Stack.MostRecentProperty = nullptr;
	Stack.Step(Stack.Object, NULL);
	FProperty* StructProp = Stack.MostRecentProperty;
	void* StructPtr = Stack.MostRecentPropertyAddress;

	P_GET_OBJECT(UChooserTable, ChooserTable);
	P_GET_UBOOL_REF(bIsValid);
	P_GET_OBJECT(UClass, OutputClass);

	P_FINISH;

	if (!StructProp || !StructPtr)
	{
		bIsValid = false;
		*(UAnimSequenceBase**)RESULT_PARAM = nullptr;
		return;
	}
    
	FStructProperty* StructProperty = CastField<FStructProperty>(StructProp);
	if (StructProperty && StructProperty->Struct)
	{
		FInstancedStruct ChooserData;
		ChooserData.InitializeAs(StructProperty->Struct, (uint8*)StructPtr);
        
		UAnimSequenceBase* RetVal = Generic_GetAnimFromChooser(TargetActor, ChooserData, ChooserTable, bIsValid, OutputClass);
		*(UAnimSequenceBase**)RESULT_PARAM = RetVal;
	}
	else
	{
		bIsValid = false;
		*(UAnimSequenceBase**)RESULT_PARAM = nullptr;
	}
}
