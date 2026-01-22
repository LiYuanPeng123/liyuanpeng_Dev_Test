#include "NumberPops/CancerNumberPopComponent_Text.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


UCancerNumberPopComponent_Text::UCancerNumberPopComponent_Text()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCancerNumberPopComponent_Text::AddNumberPop(const FCancerNumberPopRequest& NewRequest)
{
	if (!bIsShowDamage) return;

	int32 LocalDamage = NewRequest.NumberToDisplay;

	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}
	if (auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector2D ScreenLocation;
		UGameplayStatics::ProjectWorldToScreen(PC, NewRequest.WorldLocation, ScreenLocation);
		if (TextWidgetClass)
		{
			if (UCancerPopText* CreatedWidget = CreateWidget<UCancerPopText>(PC, TextWidgetClass))
			{
				CreatedWidget->AddToViewport();
				CreatedWidget->SetPositionInViewport(ScreenLocation);
				CreatedWidget->SetTextValue(NewRequest.NumberToDisplay);
				CreatedWidget->SetTextColor(NewRequest.bIsCriticalDamage ? CriticalDamageColor : DamageColor);
			}
		}
	}
}
