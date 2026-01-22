#include "CancerEditor.h"

#include "CancerEditorLibrary.h"
#include "ToolMenus.h"
#include "Framework/Commands/Commands.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Modules/ModuleManager.h"
#include "IDetailsView.h"
#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "Editor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "FCancerEditorModule"

static const FName CancerRedirectTagTabId(TEXT("CancerEditor_RedirectTag"));

// Command class
class FCancerEditorCommands : public TCommands<FCancerEditorCommands>
{
public:
	FCancerEditorCommands()
		: TCommands<FCancerEditorCommands>(TEXT("CancerEditor"), NSLOCTEXT("Contexts", "CancerEditor", "Cancer Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(EditRedirectTag, "Redirect Tag", "Open the Redirect Tag Tool", EUserInterfaceActionType::Button, FInputChord());
	}

	TSharedPtr<FUICommandInfo> EditRedirectTag;
};

void FCancerEditorModule::StartupModule()
{
	FCancerEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCancerEditorCommands::Get().EditRedirectTag,
		FExecuteAction::CreateRaw(this, &FCancerEditorModule::OnEditRedirectTagClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCancerEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CancerRedirectTagTabId, FOnSpawnTab::CreateRaw(this, &FCancerEditorModule::SpawnRedirectTagTab))
		.SetDisplayName(NSLOCTEXT("CancerEditor", "RedirectTagTitleTabMenu", "Redirect Gameplay Tag"))
		.SetTooltipText(NSLOCTEXT("CancerEditor", "RedirectTagTitleTabTooltip", "Open the Redirect Gameplay Tag tool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FCancerEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FCancerEditorCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CancerRedirectTagTabId);
}

TSharedRef<SDockTab> FCancerEditorModule::SpawnRedirectTagTab(const FSpawnTabArgs& Args)
{
	TSharedPtr<FStructOnScope> StructData = MakeShareable(new FStructOnScope(FCancerGameplayTagRedirectContainer::StaticStruct()));

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ENameAreaSettings::HideNameArea;
	FStructureDetailsViewArgs StructViewArgs;
	TSharedPtr<IStructureDetailsView> StructureView = PropertyEditorModule.CreateStructureDetailView(DetailsViewArgs, StructViewArgs, StructData);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(NSLOCTEXT("CancerEditor", "RedirectTagTitleTab", "Redirect Gameplay Tag"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				StructureView->GetWidget().ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.Text(NSLOCTEXT("CancerEditor", "Execute", "Execute"))
				.OnClicked_Lambda([StructData]()
				{
					const FCancerGameplayTagRedirectContainer* RedirectPtr = (const FCancerGameplayTagRedirectContainer*)StructData->GetStructMemory();
					if (RedirectPtr)
					{
						UCancerEditorLibrary::Edit_RedirectTag(RedirectPtr->Redirects);
					}
					return FReply::Handled();
				})
			]
		];
}

void FCancerEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("CancerActions");
	Section.Label = NSLOCTEXT("CancerEditor", "CancerActions", "Cancer Actions");
	
	FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		FCancerEditorCommands::Get().EditRedirectTag,
		TAttribute<FText>(),
		TAttribute<FText>(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Transform") 
	));
	Entry.SetCommandList(PluginCommands);
    Entry.StyleNameOverride = "CalloutToolbar";
}

void FCancerEditorModule::OnEditRedirectTagClicked()
{
	// Open dockable tab without stealing focus
	TWeakPtr<SWidget> PrevFocus = FSlateApplication::Get().GetKeyboardFocusedWidget();
	FGlobalTabmanager::Get()->TryInvokeTab(CancerRedirectTagTabId);
	if (PrevFocus.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(PrevFocus.Pin());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCancerEditorModule, CancerEditor)
