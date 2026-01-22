#include"Actions/Action_AddSceneComponent.h"



void UAction_AddSceneComponent::PostExecuteAction_Implementation(AActor* InActor)
{
	Super::PostExecuteAction_Implementation(InActor);
	AttachToParent(InActor);
}

void UAction_AddSceneComponent::AttachToParent(AActor* InActor) const
{
	if (const auto SceneComponent = Cast<USceneComponent>(WeakComponent.Get()))
	{
		const FName ComponentTag = *ParentComponentTag.GetTagName().ToString();
		if (const auto InParent = InActor->FindComponentByTag<USceneComponent>(ComponentTag))
		{
			SceneComponent->AttachToComponent(InParent, FAttachmentTransformRules::KeepRelativeTransform, GetSocketName());
		}
		else
		{
			SceneComponent->AttachToComponent(InActor->GetRootComponent(),
											  FAttachmentTransformRules::KeepRelativeTransform);
		}

		SceneComponent->SetRelativeTransform(FTransform(ComponentRotation, ComponentLocation, ComponentScale));
	}
}

FName UAction_AddSceneComponent::GetSocketName() const
{
	FString LeftS, RightS;
	SocketTag.GetTagName().ToString().Split(TEXT("."), &LeftS, &RightS, ESearchCase::IgnoreCase,
												ESearchDir::FromEnd);
	return FName(RightS);
}
