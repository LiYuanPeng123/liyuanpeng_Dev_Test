#include "Action_AddWidget.h"
#include "UIExtensionSystem.h"
#include "Blueprint/UserWidget.h"
#include "Common/CancerUIFunctionLibrary.h"
#include "GameFramework/CancerWidgetEntry.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UAction_AddWidget::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	for (auto Widget : Widgets)
	{
		if (!Widget.WidgetClass.IsNull())
			OutPaths.Add(Widget.WidgetClass.ToSoftObjectPath());
	}
}

void UAction_AddWidget::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();

	ACharacter* Character = Cast<ACharacter>(GetCreatActor());

	if (UWorld* World = Character->GetWorld())
	{
		UCancerUIFunctionLibrary::OpenUI(World, Layout);
		UUIExtensionSubsystem* ExtensionSubsystem = World->GetSubsystem<UUIExtensionSubsystem>();

		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		if (!PC)
		{
			return;
		}
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		for (auto Widget : Widgets)
		{
			UCancerWidgetEntry* Entry = NewObject<UCancerWidgetEntry>();
			Entry->WidgetClass = Widget.WidgetClass.Get();
			Entry->OwnerActor = GetCreatActor();
			
			ExtensionSubsystem->RegisterExtensionAsData(
				Widget.SlotID, LocalPlayer, Entry, -1);
		}
	}
}
