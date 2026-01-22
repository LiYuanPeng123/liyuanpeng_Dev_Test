


#include "UMG/DebugConsoleUI.h"

#include "EditorLevelLibrary.h"
#include "Blueprint/UserWidget.h"
#include "CancerConsole/Public/DebugConsoleSubsystem.h"
#include "Components/ScrollBox.h"
#include "UMG/WBP_CommandItem.h"

void UDebugConsoleUI::NativeConstruct()
{
	if (!CommandItemClass)
	{
		CommandItemClass = UWBP_CommandItem::StaticClass();
	}
	if (!CommandList) return;

	if (UEditorLevelLibrary::GetPIEWorlds(true).IsEmpty())
	{
		return;
	}

	UWorld* world = UEditorLevelLibrary::GetPIEWorlds(true)[0];
	if (auto* Subsystem = UDebugConsoleSubsystem::Get(world))
	{
		const auto& Vars = Subsystem->GetAllVarMetas();
		for (const auto& Var : Vars)
		{
			auto* Item = CreateWidget<UWBP_CommandItem>(this, CommandItemClass);
			Item->Init(Var, Subsystem);
			CommandList->AddChild(Item);
		}
	}
}
