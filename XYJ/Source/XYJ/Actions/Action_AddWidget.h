#pragma once

#include "CoreMinimal.h"
#include "ActionDef.h"
#include "GameplayTagContainer.h"
#include "Actions/CancerCreatorActionBase.h"
#include "Action_AddWidget.generated.h"


class UXYJ_HUDLayout;




/**
 * 
 */
UCLASS(DisplayName="Add Widgets")
class XYJ_API UAction_AddWidget : public UCancerCreatorActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=UI, meta=(Categories="UI"))
	FGameplayTag Layout;

	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{SlotID} -> {WidgetClass}"))
	TArray<FCancerHUDElementEntry> Widgets;
	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;
	protected:
	virtual void FinalizeAfterComponent_Implementation() override;
};
