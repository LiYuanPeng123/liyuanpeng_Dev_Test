
#include "Common/CancerUIPolicy.h"
#include "CancerAssetManager.h"
#include "CancerUserWidget.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/Engine.h"
#include "Common/CancerUIManagerSubsystem.h"
#include "Common/CancerLayout.h"


// Static
UCancerUIPolicy* UCancerUIPolicy::GetGameUIPolicy(const UObject* WorldContextObject)
{
	
	if (WorldContextObject)
	{
		if (const auto World = WorldContextObject->GetWorld())
		{
			if (UCancerUIManagerSubsystem* UISubsystem = World->GetSubsystem<UCancerUIManagerSubsystem>())

			{
				return UISubsystem->GetCurrentUIPolicy();
			}
		}
	}
	return nullptr;
}

UCancerUIManagerSubsystem* UCancerUIPolicy::GetOwningUIManager() const
{
	return CastChecked<UCancerUIManagerSubsystem>(GetOuter());
}

UCancerLayout* UCancerUIPolicy::GetRootLayout(UCancerUIManagerSubsystem* UIManager)
{
	if (!RootLayout)
	{
		if (auto Class = UCancerAssetManager::Get().GetSubclass(LayoutClass) )
		{
			RootLayout = CreateWidget<UCancerLayout>(GetWorld(),Class);
			RootLayout->AddToViewport(100);
		}
	}
	return RootLayout;
}

void UCancerUIPolicy::AddWidget(FGameplayTag LayoutTag, UCancerUserWidget* Widget)
{
	if (RootLayout)
		RootLayout->AddWidget(LayoutTag, Widget);
}

void UCancerUIPolicy::RemoveWidget(UCancerUserWidget*Widget)
{
	RootLayout->RemoveWidget(Widget);
}

UWorld* UCancerUIPolicy::GetWorld() const
{
	return GetOwningUIManager()->GetWorld();
}
