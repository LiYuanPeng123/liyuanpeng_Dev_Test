
#include "Action_DataAssetTemplate.h"
#include "GameFramework/Pawn.h"

void UAction_DataAsset::PostLoad()
{
	Super::PostLoad();
	if (ActorClass && ActorClass->IsChildOf(APawn::StaticClass()))
	{
		bIsPawn = true;
	}
	else
	{
		bIsPawn = false;
	}
}

#if WITH_EDITOR
void UAction_DataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UAction_DataAsset, ActorClass) ||
		PropertyChangedEvent.Property == nullptr) // Handle generic changes
	{
		if (ActorClass && ActorClass->IsChildOf(APawn::StaticClass()))
		{
			bIsPawn = true;
		}
		else
		{
			bIsPawn = false;
		}
	}
}
#endif
