
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CancerConsole/Public/DebugConsoleSubsystem.h"

#include "WBP_CommandItem.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CANCERCONSOLEEDITOR_API UWBP_CommandItem : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(const FDebugVarMeta Var, UDebugConsoleSubsystem* Subsystem);

protected:
	UPROPERTY(meta=(BindWidget)) UTextBlock* NameText;
	UPROPERTY(meta=(BindWidget)) UTextBlock* ValueText;
	UPROPERTY(meta=(BindWidget)) UButton* ExecButton;

private:
	FString CommandName;
	bool bIsBoolLike = false;
	UDebugConsoleSubsystem* DebugSubsystem = nullptr;

	UFUNCTION()
	void OnExecute();
};
