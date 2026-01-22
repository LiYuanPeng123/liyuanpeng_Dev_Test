
#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"

#include "DebugConsoleUI.generated.h"


class UUserWidget;
class UScrollBox;

UCLASS()
class CANCERCONSOLEEDITOR_API UDebugConsoleUI : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* CommandList;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> CommandItemClass;
};
