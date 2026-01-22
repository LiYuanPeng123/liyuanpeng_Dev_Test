
#include "Common/CancerLayout.h"
#include "CancerUserWidget.h"
#include "Common/CancerUIManagerSubsystem.h"
#include "Common/CancerUIPolicy.h"
#include "Common/CancerWidgetStack.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/GameInstance.h"


class UObject;

UCancerLayout* UCancerLayout::GetPrimaryGameLayout(const UObject* WorldContextObject)
{
	if (WorldContextObject)
    	{
    		if (const auto World = WorldContextObject->GetWorld())
    		{
    			if (UCancerUIManagerSubsystem* UISubsystem = World->GetSubsystem<UCancerUIManagerSubsystem>())
    
    			{
    				if (UCancerUIPolicy* Policy = UISubsystem->GetCurrentUIPolicy())
    				{
    					if (UCancerLayout* RootLayout = Policy->GetRootLayout(UISubsystem))
    					{
    						return RootLayout;
    					}
    				}
    			}
    		}
    	}
	return nullptr;
}


UCancerLayout::UCancerLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCancerLayout::RegisterLayer(FGameplayTag LayerTag, UCancerWidgetStack* LayerWidget)
{
	if (!IsDesignTime())
	{
		Layers.Add(LayerTag, LayerWidget);
	}
}

UCancerWidgetStack* UCancerLayout::GetLayerWidget(FGameplayTag LayerName)
{
	return Layers.FindRef(LayerName);
}

void UCancerLayout::AddWidget(FGameplayTag LayerTag, UCancerUserWidget* Widget)
{
	if (auto Layer = GetLayerWidget(LayerTag))
	{
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Layer->AddChild(Widget));
		CanvasPanelSlot->SetAnchors(FAnchors(0, 0, 1, 1));
		CanvasPanelSlot->SetOffsets(FMargin(0, 0, 0, 0));
	}
}


void UCancerLayout::RemoveWidget(UCancerUserWidget* Widget)
{
	for (auto LayerKVP : Layers)
	{
		LayerKVP.Value->RemoveChild(Widget);
	}
}


