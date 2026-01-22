#include "CancerInputSubsystem.h"

#include "CancerInputLog.h"
#include "CancerInputSettings.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

FInputActionMappingData::FInputActionMappingData()
{
	InputMappingContext = nullptr;
	Guid = FGuid::NewGuid().ToString();
}

FInputActionMappingData::FInputActionMappingData(UInputMappingContext* InputMappingContext, FGameplayTag InputLayer)
{
	this->InputMappingContext = InputMappingContext;
	if (InputLayer.IsValid())
	{
		this->InputLayer = InputLayer;
	}
	else
	{
		UE_LOG(CancerInputLog, Error, TEXT("添加的输入映射输入层级为空"));
		check(false);
	}
	Guid = FGuid::NewGuid().ToString();
}

void UCancerInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCancerInputSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);
}

UCancerInputSubsystem* UCancerInputSubsystem::GetCancerInputSubsystem(const AController* Controller)
{
	if (IsValid(Controller))
	{
		const APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController))
		{
			const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				return LocalPlayer->GetSubsystem<UCancerInputSubsystem>();
			}
		}
	}
	return nullptr;
}

FString UCancerInputSubsystem::ActivateInputMappingContext(FInputActionMappingData InputActionMappingData)
{
	ensureMsgf(InputActionMappingData.InputLayer.IsValid(), TEXT("添加的输入映射输入层级为空"));
	ensureMsgf(IsValid(InputActionMappingData.InputMappingContext), TEXT("添加的输入映射输入映射为空"));

	InputActionDataMappingMap.Add(InputActionMappingData.Guid, InputActionMappingData);
	UpdateInputMappingContext();
	return InputActionMappingData.Guid;
}

void UCancerInputSubsystem::DeactivateInputMappingContext(FString InputGuid)
{
	if (InputActionDataMappingMap.Contains(InputGuid))
	{
		InputActionDataMappingMap.Remove(InputGuid);
		UpdateInputMappingContext();
	}
}

void UCancerInputSubsystem::UpdateInputMappingContext()
{
	TArray<FInputActionMappingData> InputActionMappingDataArray;
	for (int i = 0; i < GetDefault<UCancerInputSettings>()->InputLayerDataArray.Num(); ++i)
	{
		FInputLayerData InputLayerData = GetDefault<UCancerInputSettings>()->InputLayerDataArray[i];
		TArray<FString> Keys;
		InputActionDataMappingMap.GetKeys(Keys);
		bool FindInput = false;
		for (int j = Keys.Num() - 1; j >= 0; --j)
		{
			FString Key = Keys[j];
			FInputActionMappingData InputActionMappingData = InputActionDataMappingMap[Key];
			if (InputActionMappingData.InputLayer == InputLayerData.InputLayer)
			{
				FindInput = true;
				InputActionMappingDataArray.Add(InputActionMappingData);
			}
		}
		if (FindInput && InputLayerData.bBlockLowInputLayer)
		{
			//阻挡低低优先级输入
			break;
		}
	}
	//清理映射重新映射
	UWorld* World = GetWorld();
	if (!World) return;
	ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	if (!LocalPlayer) return;
	auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!InputSubsystem)return;
	InputSubsystem->ClearAllMappings();
	int32 Priority = 0;
	for (int i = InputActionMappingDataArray.Num() - 1; i >= 0; --i)
	{
		FInputActionMappingData InputActionMappingData = InputActionMappingDataArray[i];
		InputSubsystem->AddMappingContext(InputActionMappingData.InputMappingContext, Priority);
		Priority++;
	}
}
