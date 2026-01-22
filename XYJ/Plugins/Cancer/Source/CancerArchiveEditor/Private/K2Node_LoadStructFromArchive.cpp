#include "K2Node_LoadStructFromArchive.h"
#include "CancerArchiveBlueprintLibrary.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

void UK2Node_LoadStructFromArchive::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, FName("Exec"));
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FName("Key"));
	UEdGraphPin* StructTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FName("StructType"));
	StructTypePin->PinType.PinSubCategoryObject = nullptr; // wildcard for type dropdown
	UEdGraphPin* StructOut = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FName("Struct"));
	StructOut->PinType = StructTypePin->PinType;
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FName("Succeeded"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FName("Failed"));
}

FText UK2Node_LoadStructFromArchive::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Load Struct From Archive"));
}

FText UK2Node_LoadStructFromArchive::GetMenuCategory() const
{
	return FText::FromString(TEXT("CancerArchive|Struct"));
}

void UK2Node_LoadStructFromArchive::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
	ActionRegistrar.AddBlueprintAction(GetClass(), Spawner);
}

void UK2Node_LoadStructFromArchive::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* SucceededPin = FindPinChecked(FName("Succeeded"), EGPD_Output);
	UEdGraphPin* FailedPin = FindPinChecked(FName("Failed"), EGPD_Output);
	UEdGraphPin* KeyPin = FindPinChecked(FName("Key"), EGPD_Input);
	UEdGraphPin* StructTypePin = FindPinChecked(FName("StructType"), EGPD_Input);
	UEdGraphPin* StructOutPin = FindPinChecked(FName("Struct"), EGPD_Output);

	// Validate type selection
	if (StructTypePin->PinType.PinSubCategoryObject == nullptr)
	{
		CompilerContext.MessageLog.Error(TEXT("@@ requires a Struct Type to be selected"), this);
		BreakAllNodeLinks();
		return;
	}

	UK2Node_CallFunction* Call = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	Call->SetFromFunction(UCancerArchiveBlueprintLibrary::StaticClass()->FindFunctionByName(FName("GetStructFromArchive")));
	Call->AllocateDefaultPins();

	UEdGraphPin* CallExec = Call->GetExecPin();
	UEdGraphPin* CallThen = Call->GetThenPin();
	UEdGraphPin* CallKey = Call->FindPinChecked(FName("Key"));
	UEdGraphPin* CallStruct = Call->FindPinChecked(FName("OutStruct"));
	UEdGraphPin* CallStructType = Call->FindPinChecked(FName("StructType"));
	UEdGraphPin* CallIsValid = Call->FindPinChecked(FName("bIsValid"));

	Schema->TryCreateConnection(ExecPin, CallExec);
	Schema->TryCreateConnection(KeyPin, CallKey);
	// propagate struct type from input type pin to output and call pin
	StructOutPin->PinType = StructTypePin->PinType;

	CallStruct->PinType = StructOutPin->PinType;
	Schema->TryCreateConnection(StructOutPin, CallStruct);
	// set StructType default object for DeterminesOutputType
	if (UScriptStruct* SelectedStruct = Cast<UScriptStruct>(StructTypePin->PinType.PinSubCategoryObject))
	{
		CallStructType->DefaultObject = SelectedStruct;
	}

	UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	Branch->AllocateDefaultPins();
	Schema->TryCreateConnection(CallThen, Branch->GetExecPin());
	Schema->TryCreateConnection(CallIsValid, Branch->GetConditionPin());
	Schema->TryCreateConnection(Branch->GetThenPin(), SucceededPin);
	Schema->TryCreateConnection(Branch->GetElsePin(), FailedPin);

	BreakAllNodeLinks();
}

void UK2Node_LoadStructFromArchive::ReconstructNode()
{
	Super::ReconstructNode();
	if (UEdGraphPin* StructTypePin = FindPin(FName("StructType"), EGPD_Input))
	{
		if (UEdGraphPin* StructOutPin = FindPin(FName("Struct"), EGPD_Output))
		{
			StructOutPin->PinType = StructTypePin->PinType;
		}
	}
}

void UK2Node_LoadStructFromArchive::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
	UEdGraphPin* NewStructTypePin = FindPin(FName("StructType"), EGPD_Input);
	UEdGraphPin* NewStructOutPin = FindPin(FName("Struct"), EGPD_Output);
	if (NewStructTypePin && NewStructOutPin)
	{
		NewStructOutPin->PinType = NewStructTypePin->PinType;
	}
}
