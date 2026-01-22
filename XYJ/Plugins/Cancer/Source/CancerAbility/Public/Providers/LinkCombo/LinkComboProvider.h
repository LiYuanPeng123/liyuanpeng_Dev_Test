#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "LinkComboProvider.generated.h"

class UInputAction;
/**
 *  连招派生
 */
UCLASS(Const, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced,
	meta = (DisplayName = "Link Combo Provider"))
class CANCERABILITY_API ULinkComboProvider : public UObject
{
	GENERATED_BODY()

public:
	ULinkComboProvider();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="派生连招")
	FGameplayTag LinkComboTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="输入操作")
	UInputAction* InputAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", DisplayName="命中触发")
	bool bISHit{true};
};
