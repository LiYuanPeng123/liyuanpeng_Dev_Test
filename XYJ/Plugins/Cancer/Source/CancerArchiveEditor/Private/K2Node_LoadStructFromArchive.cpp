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
	
	// Create StructType input pin to allow dynamic connections
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UScriptStruct::StaticClass(), FName("StructType"));

	UEdGraphPin* StructOut = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FName("Struct"));
	if (StructType)
	{
		StructOut->PinType.PinSubCategoryObject = StructType;
	}
	else
	{
		StructOut->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		StructOut->PinType.PinSubCategoryObject = nullptr;
	}
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FName("Succeeded"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FName("Failed"));
}

void UK2Node_LoadStructFromArchive::PostReconstructNode()
{
	Super::PostReconstructNode();
	RefreshOutputStructType();
}

FText UK2Node_LoadStructFromArchive::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraphPin* StructTypePin = FindPin(FName("StructType"));
	if (StructTypePin && StructTypePin->LinkedTo.Num() > 0)
	{
		return FText::FromString(TEXT("Load Struct From Archive (Dynamic)"));
	}

	if (StructType)
	{
		return FText::Format(NSLOCTEXT("K2Node", "LoadStructFromArchive_Title", "Load Struct {0} From Archive"), StructType->GetDisplayNameText());
	}
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
	UEdGraphPin* StructOutPin = FindPinChecked(FName("Struct"), EGPD_Output);
	UEdGraphPin* StructTypePin = FindPinChecked(FName("StructType"), EGPD_Input);

	// Validate type selection
	UScriptStruct* SelectedStruct = StructType;
	if (StructTypePin->DefaultObject)
	{
		SelectedStruct = Cast<UScriptStruct>(StructTypePin->DefaultObject);
	}
	else if (!StructTypePin->DefaultValue.IsEmpty())
	{
		FString Path = StructTypePin->DefaultValue;
		SelectedStruct = FindObject<UScriptStruct>(nullptr, *Path, true);
		if (!SelectedStruct)
		{
			SelectedStruct = LoadObject<UScriptStruct>(nullptr, *Path);
		}
	}

	if (StructTypePin->LinkedTo.Num() == 0 && SelectedStruct == nullptr)
	{
		CompilerContext.MessageLog.Error(TEXT("@@ requires a Struct Type to be selected in Details or connected via Pin"), this);
		BreakAllNodeLinks();
		return;
	}

	UK2Node_CallFunction* Call = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	Call->SetFromFunction(UCancerArchiveBlueprintLibrary::StaticClass()->FindFunctionByName(FName("GetStructFromArchive")));
	Call->AllocateDefaultPins();

	UEdGraphPin* CallExec = Call->GetExecPin();
	UEdGraphPin* CallThen = Call->GetThenPin();
	UEdGraphPin* CallKey = Call->FindPinChecked(FName("Key"));
	UEdGraphPin* CallStructType = Call->FindPinChecked(FName("StructType"));
	UEdGraphPin* CallIsValid = Call->FindPinChecked(FName("bIsValid"));

	// propagate struct type from input type pin to output and call pin
	CallStructType->PinType = StructOutPin->PinType;

	CompilerContext.MovePinLinksToIntermediate(*ExecPin, *CallExec);
	CompilerContext.MovePinLinksToIntermediate(*KeyPin, *CallKey);
	CompilerContext.MovePinLinksToIntermediate(*StructOutPin, *CallStructType);

	UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	Branch->AllocateDefaultPins();
	Schema->TryCreateConnection(CallThen, Branch->GetExecPin());
	Schema->TryCreateConnection(CallIsValid, Branch->GetConditionPin());
	CompilerContext.MovePinLinksToIntermediate(*SucceededPin, *Branch->GetThenPin());
	CompilerContext.MovePinLinksToIntermediate(*FailedPin, *Branch->GetElsePin());

	BreakAllNodeLinks();
}

void UK2Node_LoadStructFromArchive::ReconstructNode()
{
	Super::ReconstructNode();
}

void UK2Node_LoadStructFromArchive::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);
}

void UK2Node_LoadStructFromArchive::RefreshOutputStructType()
{
	UEdGraphPin* StructTypePin = FindPin(FName("StructType"));
	UEdGraphPin* StructOut = FindPin(FName("Struct"));

	if (StructTypePin && StructOut)
	{
		UScriptStruct* SelectedStruct = nullptr;

		// 1. Check Connection
		if (StructTypePin->LinkedTo.Num() > 0)
		{
			// Wildcard if connected
		}
		// 2. Check Pin Default Object
		else if (StructTypePin->DefaultObject)
		{
			SelectedStruct = Cast<UScriptStruct>(StructTypePin->DefaultObject);
		}
		// 3. Fallback to Property
		if (!SelectedStruct && StructType)
		{
			SelectedStruct = StructType;
		}

		// Apply
		if (SelectedStruct && StructTypePin->LinkedTo.Num() == 0)
		{
			if (StructOut->PinType.PinSubCategoryObject != SelectedStruct)
			{
				if (StructOut->SubPins.Num() > 0)
				{
					GetSchema()->RecombinePin(StructOut);
				}

				StructOut->PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
				StructOut->PinType.PinSubCategoryObject = SelectedStruct;
			}
		}
		else
		{
			if (StructOut->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
			{
				StructOut->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
				StructOut->PinType.PinSubCategoryObject = nullptr;
			}
		}
	}
}

void UK2Node_LoadStructFromArchive::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	if (Pin->PinName == FName("StructType"))
	{
		RefreshOutputStructType();
	}
}

void UK2Node_LoadStructFromArchive::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);
	if (Pin->PinName == FName("StructType"))
	{
		RefreshOutputStructType();
	}
}

void UK2Node_LoadStructFromArchive::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UK2Node_LoadStructFromArchive, StructType))
	{
		ReconstructNode();
	}
}
