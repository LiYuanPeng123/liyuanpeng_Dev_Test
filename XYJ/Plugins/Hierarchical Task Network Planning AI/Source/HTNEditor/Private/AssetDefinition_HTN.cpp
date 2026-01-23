// Copyright 2020-2025 Maksym Maisak. All Rights Reserved.

#include "AssetDefinition_HTN.h"
#include "HTN.h"
#include "HTNColors.h"
#include "HTNEditorModule.h"
#include "SHTNDiff.h"

#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "AssetTypeDefinition_HTN"

FText UAssetDefinition_HTN::GetAssetDisplayName() const
{
	return LOCTEXT("HTN", "Hierarchical Task Network");
}

FLinearColor UAssetDefinition_HTN::GetAssetColor() const
{
	return HTNColors::Asset.ToFColor(/*bSRGB=*/true);
}

TSoftClassPtr<> UAssetDefinition_HTN::GetAssetClass() const
{
	return UHTN::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_HTN::GetAssetCategories() const
{
	static const FAssetCategoryPath Categories[] = { FAssetCategoryPath(EAssetCategoryPaths::AI) };
	return Categories;
}

EAssetCommandResult UAssetDefinition_HTN::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	const EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();
	IHTNEditorModule& EditorModule = IHTNEditorModule::Get();
	
	for (UObject* const Obj : OpenArgs.LoadObjects<UObject>())
	{
		if (UHTN* const HTN = Cast<UHTN>(Obj))
		{
			EditorModule.CreateHTNEditor(Mode, OpenArgs.ToolkitHost, HTN);
		}
	}
	return EAssetCommandResult::Handled;
}

EAssetCommandResult UAssetDefinition_HTN::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	UHTN* const OldHTN = Cast<UHTN>(DiffArgs.OldAsset);
	UHTN* const NewHTN = Cast<UHTN>(DiffArgs.NewAsset);
	check(NewHTN || OldHTN);

	const bool bIsSingleAsset = !OldHTN || !NewHTN || OldHTN->GetName() == NewHTN->GetName();
	const FText WindowTitle = bIsSingleAsset ? 
		FText::Format(LOCTEXT("HTNDiff", "{0} - HTN Diff"), FText::FromString(NewHTN->GetName())) : 
		LOCTEXT("NamelessHTNDiff", "HTN Diff");

	const TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(FVector2D(1000, 800));

	Window->SetContent(SNew(SHTNDiff)
		.HTNOld(OldHTN)
		.HTNNew(NewHTN)
		.OldRevision(DiffArgs.OldRevision)
		.NewRevision(DiffArgs.NewRevision)
		.ShowAssetNames(!bIsSingleAsset)
		.OpenInDefaultDiffTool_UObject(this, &ThisClass::OpenInDefaultDiffTool));

	if (const TSharedPtr<SWindow> ActiveModal = FSlateApplication::Get().GetActiveModalWindow())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window, ActiveModal.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window);
	}

	return EAssetCommandResult::Handled;
}

void UAssetDefinition_HTN::OpenInDefaultDiffTool(UHTN* OldHTN, UHTN* NewHTN) const
{
	FAssetDiffArgs DiffArgs;
	DiffArgs.OldAsset = OldHTN;
	DiffArgs.NewAsset = NewHTN;
	Super::PerformAssetDiff(DiffArgs);
}

#undef LOCTEXT_NAMESPACE
