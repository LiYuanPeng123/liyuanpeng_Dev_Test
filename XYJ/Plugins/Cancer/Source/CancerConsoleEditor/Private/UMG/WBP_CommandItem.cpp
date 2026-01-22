

#include "UMG/WBP_CommandItem.h"

#include "CancerConsole/Public/DebugConsoleSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UWBP_CommandItem::Init(const FDebugVarMeta Var, UDebugConsoleSubsystem* Subsystem)
{
	NameText->SetText(FText::FromString(Var.DisplayName));
	bIsBoolLike = true;
	ValueText->SetText(
		FText::FromString(DebugSubsystem->GetVarAsBool(CommandName) ? TEXT("true") : TEXT("false")));
	ExecButton->OnClicked.AddDynamic(this, &UWBP_CommandItem::OnExecute);
}

void UWBP_CommandItem::OnExecute()
{
	if (bIsBoolLike)
	{
		bool NewValue = !DebugSubsystem->GetVarAsBool(CommandName);
		DebugSubsystem->SetVarFromString(CommandName, NewValue ? TEXT("1") : TEXT("0"));
		ValueText->SetText(FText::FromString(NewValue ? TEXT("true") : TEXT("false")));
	}

	DebugSubsystem->ExecuteCommand(CommandName);
}
