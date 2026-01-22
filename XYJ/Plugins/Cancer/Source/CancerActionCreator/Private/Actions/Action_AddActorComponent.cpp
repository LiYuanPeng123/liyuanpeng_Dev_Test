#include "Actions/Action_AddActorComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"


DEFINE_LOG_CATEGORY(LogAction)

void UAction_AddActorComponent::PostInitComponent_Implementation()
{
}

void UAction_AddActorComponent::FinalizeAfterComponent_Implementation()
{
	auto Component = GetCreateComponent();
	check(Component);
	if (Component)
		Component->RegisterComponent();
}

void UAction_AddActorComponent::PostExecuteAction_Implementation(AActor* InActor)
{
	Super::PostExecuteAction_Implementation(InActor);
	WeakComponent = CreateComponent(InActor);
}

TArray<FName> UAction_AddActorComponent::GetComponentTags() const
{
	TArray<FName> TagArray;
	for (FGameplayTag GameplayTag : ComponentTags.GetGameplayTagArray())
	{
		FName TagName = GetGameplayTagLastName(GameplayTag);
		TagArray.Add(TagName);
	}
	return TagArray;
}

FName UAction_AddActorComponent::GetGameplayTagLastName(FGameplayTag Tag)
{
	FString LeftS, RightS;
	Tag.GetTagName().ToString().Split(TEXT("."), &LeftS, &RightS, ESearchCase::IgnoreCase,
	                                  ESearchDir::FromEnd);
	return *RightS;
}

UActorComponent* UAction_AddActorComponent::GetCreateComponent() const
{
	return WeakComponent.IsValid() ? WeakComponent.Get() : nullptr;
}

UActorComponent* UAction_AddActorComponent::CreateComponent(AActor* InActor)
{

	
	if (!ComponentClass)
	{
		check(ComponentClass);
		UE_LOG(LogAction, Error, TEXT("组件添加失败：组件类为空"));
		return nullptr;
	}
	//TODO::默认传入InActor 应该为Pawn
	AActor* TargetActor = InActor;
	switch (CancerActionType)
	{
	case ECancerActionType::Character:
		if (AController* AsController = Cast<AController>(InActor))
		{
			if (APawn* Pawn = AsController->GetPawn())
			{
				TargetActor = Pawn;
			}
			else
			{
				UE_LOG(LogAction, Error, TEXT("组件添加失败：控制器未持有Pawn"));
				return nullptr;
			}
		}
		break;
	case ECancerActionType::Controller:
		if (APawn* AsPawn = Cast<APawn>(InActor))
		{
			if (AController* Controller = AsPawn->GetController())
			{
				TargetActor = Controller;
			}
			else
			{
				UE_LOG(LogAction, Error, TEXT("组件添加失败：角色未持有Controller"));
				return nullptr;
			}
		}
		break;
	default:
		break;
	}

	//如果已经存在返回该组件
	if (UActorComponent* Component = TargetActor->FindComponentByClass(ComponentClass))
	{
		Component->ComponentTags = GetComponentTags();
		UE_LOG(LogAction, Warning, TEXT("组件已经存在：Component: %s"), *Component->GetName());
		return Component;
	}

	//FName ComponentName = GetComponentName(InActor);
	UActorComponent* Component = NewObject<UActorComponent>(
		TargetActor, ComponentClass, ComponentNameOverride, RF_Transient);
	
	TargetActor->AddInstanceComponent(Component);

	Component->ComponentTags = GetComponentTags();
	Component->bAllowAnyoneToDestroyMe = true;
	Component->CreationMethod = EComponentCreationMethod::Instance;
	UE_LOG(LogAction, Warning, TEXT("组件添加成功：Component: %s"), *Component->GetName());
	return Component;
}

void UAction_AddActorComponent::PreExecuteAction_Implementation(AActor* InActor)
{
	Super::PreExecuteAction_Implementation(InActor);
}
