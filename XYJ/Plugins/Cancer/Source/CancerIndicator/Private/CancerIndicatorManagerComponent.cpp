#include "CancerIndicatorManagerComponent.h"
#include "CancerIndicatorSourceComponent.h"
#include "CancerIndicatorSubsystem.h"
#include "IndicatorDescriptor.h"
#include "IndicatorWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"

UCancerIndicatorManagerComponent::UCancerIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

UCancerIndicatorManagerComponent* UCancerIndicatorManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<UCancerIndicatorManagerComponent>();
	}

	return nullptr;
}

void UCancerIndicatorManagerComponent::HandleIndicatorComponent(
	UCancerIndicatorSourceComponent* IndicatorSourceComponent, bool bAdd)
{
	if (IndicatorSourceComponent && bAdd)
	{
		AddIndicator(IndicatorSourceComponent->GetIndicator());
	}
	else
	{
		RemoveIndicator(IndicatorSourceComponent->GetIndicator());
	}
}


void UCancerIndicatorManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UCancerIndicatorSubsystem* IndicatorSubsystem = GetWorld()->GetSubsystem<UCancerIndicatorSubsystem>())
	{
		for (auto Component : IndicatorSubsystem->GetIndicatorComponents())
		{
			if (Component)
			{
				AddIndicator(Component->GetIndicator());
			}
		}
		IndicatorSubsystem->OnIndicatorChange.AddDynamic(this, &ThisClass::HandleIndicatorComponent);
	}
}

void UCancerIndicatorManagerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (UIndicatorDescriptor* Descriptor : Indicators)
	{
		//需要自动更新显示
		if (Descriptor && Descriptor->GetAutoUpdateVisible())
		{
			bool bShow = false;
			float MaxDis = Descriptor->GetMaxDis();
			if (AActor* DescriptorOwnerActor = Cast<AActor>(Descriptor->GetOuter()))
			{
				if (APlayerController* Controller = GetController<APlayerController>())
				{
					float Dis = UKismetMathLibrary::Vector_Distance(Controller->GetPawn()->GetActorLocation(),
					                                                DescriptorOwnerActor->GetActorLocation());
					if (Dis <= MaxDis)
					{
						bShow = true;
					}
					Descriptor->SetDis(Dis);
				}
			}
			Descriptor->SetDesiredVisibility(bShow);
		}
		if (auto Widget = Descriptor->IndicatorWidget.Get())
		{
			if (Widget->Implements<UIndicatorWidgetInterface>())
			{
				//更新控件的显示
				IIndicatorWidgetInterface::Execute_UpdateIndicatorVisible(Widget, Widget,
				                                                          Descriptor);
			}
		}
	}
}

void UCancerIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UCancerIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);

		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
