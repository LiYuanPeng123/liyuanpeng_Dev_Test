#pragma once
#include "CoreMinimal.h"
#include "Actions/CancerCreatorActionBase.h"
#include "GameplayTagContainer.h"
#include "Actions/Action_AddActorComponent.h"
#include "Action_AddQuickBarComponent.generated.h"

class UCancerQuickBarInstance;
class UCancerQuickBarComponent;


UCLASS(DisplayName="武器快捷插槽")
class XYJ_API UAction_AddQuickBarComponent : public UAction_AddActorComponent
{
    GENERATED_BODY()
public:
    
    virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
    virtual void PostInitComponent_Implementation() override;

    virtual void FinalizeAfterComponent_Implementation() override;
};