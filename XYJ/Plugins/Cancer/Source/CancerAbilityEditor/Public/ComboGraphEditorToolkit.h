#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class UComboGraph;
class UEdGraph;
class IDetailsView;
	class SGraphEditor;
class UMoveDefinition;

class FComboGraphEditorToolkit : public FAssetEditorToolkit
{
public:
	void InitEditor(UComboGraph* InGraphAsset);

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

private:
	UComboGraph* GraphAsset = nullptr;
	UEdGraph* EdGraph = nullptr;
	TSharedPtr<IDetailsView> DetailsWidget;
		TSharedPtr<SGraphEditor> GraphEditorWidget;
	void CreateGraphEditor();
	void OnGraphSelectionChanged(const TSet<UObject*>& NewSelection);
	void LoadAssetIntoEdGraph();
	void BuildAssetFromEdGraph();
		void ConnectSelectedMoves();
		void ValidateGraph();
		void AutoLayout();
		void CreateMoveNodesFromContentBrowserSelection();
};

