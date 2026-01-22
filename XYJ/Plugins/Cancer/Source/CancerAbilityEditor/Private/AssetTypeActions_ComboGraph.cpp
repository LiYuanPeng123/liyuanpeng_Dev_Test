#include "AssetTypeActions_ComboGraph.h"
#include "CancerAbility/Public/Combo/ComboGraph.h"
#include "ComboGraphEditorToolkit.h"

FText FAssetTypeActions_ComboGraph::GetName() const
{
	return FText::FromString(TEXT("Combo Graph"));
}

FColor FAssetTypeActions_ComboGraph::GetTypeColor() const
{
	return FColor(80, 160, 255);
}

UClass* FAssetTypeActions_ComboGraph::GetSupportedClass() const
{
	return UComboGraph::StaticClass();
}

uint32 FAssetTypeActions_ComboGraph::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

void FAssetTypeActions_ComboGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Obj : InObjects)
	{
		if (UComboGraph* Graph = Cast<UComboGraph>(Obj))
		{
			TSharedRef<FComboGraphEditorToolkit> Editor = MakeShared<FComboGraphEditorToolkit>();
			Editor->InitEditor(Graph);
		}
	}
}

