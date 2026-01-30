#include "CancerActionLibrary.h"
#include "Action_DataAssetTemplate.h"
#include "CancerActionInterface.h"
#include "CancerActorSubsystem.h"
#include "CancerAssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


DEFINE_LOG_CATEGORY_STATIC(LogAction, Log, All);

void UCancerActionLibrary::LoadActionData(TSoftObjectPtr<UAction_DataAsset> Data, AActor* Actor,
                                          FOnActionCompled OnCompleted)
{
	if (Data.IsNull() || !Actor)
		return;

	auto ActionData = Data.LoadSynchronous();
	//加载依赖资产
	TArray<FSoftObjectPath> Paths;
	ActionData->GatherSoftReferences(Paths);
	if (Paths.Num() > 0)
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(Paths, FStreamableDelegate());
	}

	//加载组件
	auto Temp = ActionData->StartAsyncLoadAndExecuteWithHandle(
		Actor, FSimpleDelegate::CreateLambda([Actor,OnCompleted, ActionData]()
		{
			ActionData->PreInitComponent();
			ActionData->PostInitComponent();
			ActionData->FinalizeAfterComponent();

			//角色生成时的处理
			if (ACharacter* Character = Cast<ACharacter>(Actor))
			{
				if (auto MovementComponent = Character->
					GetCharacterMovement())
					Character->OnMovementModeChanged(
						MovementComponent->MovementMode,
						MovementComponent->CustomMovementMode);

				UAIPerceptionStimuliSourceComponent* Stimuli = Character->FindComponentByClass<
					UAIPerceptionStimuliSourceComponent>();
				if (!Stimuli)
				{
					Stimuli = NewObject<UAIPerceptionStimuliSourceComponent>(Character);
					Stimuli->RegisterComponent();
				}
				if (Stimuli)
				{
					Stimuli->RegisterForSense(UAISense_Sight::StaticClass());
				}
			}

			//广播初始化已经完成
			if (Actor->Implements<UCancerActionInterface>())
			{
				ICancerActionInterface::Execute_HandleOnActionCompleted(Actor);
			}
			OnCompleted.ExecuteIfBound(Actor);

			if (auto ActorSubsystem = Actor->GetWorld()->GetSubsystem<UCancerActorSubsystem>())
			{
				
				ActorSubsystem->RegisterActor(ActionData,Actor);
			}
			
		}));
}

AActor* UCancerActionLibrary::SpawnActorFromActionData(const UObject* WorldContextObject,
                                                       TSoftObjectPtr<UAction_DataAsset> Data, FTransform SpawnTrans)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = GEngine
		                ? GEngine->GetWorldFromContextObject(WorldContextObject,
		                                                     EGetWorldErrorMode::LogAndReturnNull)
		                : WorldContextObject->GetWorld();
	if (!World) return nullptr;
	if (Data.IsNull())
	{
		UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData Data 没有配置"));
		return nullptr;
	}

	UAction_DataAsset* ActionData = UCancerAssetManager::Get().GetAsset(Data);
	if (!ActionData)
	{
		UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData Data 加载失败"));
		return nullptr;
	}

	AActor* NewActor = nullptr;
	TSubclassOf<AActor> ActorClass = ActorClass = ActionData->ActorClass;
	if (!ActorClass)
	{
		UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData ActorClass为空"));
		return nullptr;
	}

	AController* Controller = nullptr;
	FVector SpawnLocation = SpawnTrans.GetLocation(); // 默认位置
	FRotator SpawnRotation = FRotator(0, SpawnTrans.GetRotation().Rotator().Yaw, 0);
	if (ActorClass->IsChildOf(APawn::StaticClass()))
	{
		TSubclassOf<AController> ControllerClass = ControllerClass = ActionData->ControllerClass;
		if (!ControllerClass)
		{
			UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData ControllerClass为空"));
			return nullptr;
		}
		// 生成 AIController
		FActorSpawnParameters ControllerSpawnParams;
		ControllerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ControllerSpawnParams.Owner = nullptr;
		Controller = World->SpawnActor<AAIController>(
			ControllerClass,
			SpawnLocation,
			SpawnRotation,
			ControllerSpawnParams
		);
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 使用延迟构造方式生成NewActor
	FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	NewActor = World->SpawnActorDeferred<AActor>(ActorClass, SpawnTransform, nullptr,
	                                             nullptr,
	                                             ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (NewActor)
	{
		if (Controller)
		{
			if (APawn* NewPawn = Cast<APawn>(NewActor))
				Controller->Possess(NewPawn);
		}
		// 完成延迟构造
		UGameplayStatics::FinishSpawningActor(NewActor, SpawnTransform);
	}
	else
	{
		UE_LOG(LogAction, Error, TEXT("生成Actor失败"));
	}
	
	//加载组件
	LoadActionData(Data, NewActor,{});

	return NewActor;
}

AActor* UCancerActionLibrary::K2_CreatePawnFromActionData(UObject* WorldContextObject,
                                                          TSoftObjectPtr<UAction_DataAsset> Data,
                                                          const FTransform& SpawnTrans, FOnActionCompled OnCompleted,
                                                          ESpawnActorCollisionHandlingMethod CollisionHandlingMethod,
                                                          AActor* InOwner)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = GEngine
		                ? GEngine->GetWorldFromContextObject(WorldContextObject,
		                                                     EGetWorldErrorMode::LogAndReturnNull)
		                : WorldContextObject->GetWorld();
	if (!World) return nullptr;
	if (Data.IsNull())
	{
		UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData Data 没有配置"));
		return nullptr;
	}

	UAction_DataAsset* ActionData = UCancerAssetManager::Get().GetAsset(Data);
	if (!ActionData)
	{
		UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData Data 加载失败"));
		return nullptr;
	}

	AActor* NewActor = nullptr;
	TSubclassOf<AActor> ActorClass = ActorClass = ActionData->ActorClass;
	if (!ActorClass)
	{
		UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData ActorClass为空"));
		return nullptr;
	}

	AAIController* Controller = nullptr;
	FVector SpawnLocation = SpawnTrans.GetLocation(); // 默认位置
	FRotator SpawnRotation = FRotator(0, SpawnTrans.GetRotation().Rotator().Yaw, 0);
	if (ActorClass->IsChildOf(APawn::StaticClass()))
	{
		TSubclassOf<AController> ControllerClass = ControllerClass = ActionData->ControllerClass;
		if (!ControllerClass)
		{
			UE_LOG(LogAction, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData ControllerClass为空"));
			return nullptr;
		}
		// 生成 Controller
		FActorSpawnParameters ControllerSpawnParams;
		ControllerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ControllerSpawnParams.Owner = nullptr;
		Controller = World->SpawnActor<AAIController>(
			ControllerClass,
			SpawnLocation,
			SpawnRotation,
			ControllerSpawnParams
		);
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 使用延迟构造方式生成NewActor
	FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	NewActor = World->SpawnActorDeferred<AActor>(ActorClass, SpawnTransform, nullptr,
	                                             nullptr,
	                                             ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (NewActor)
	{
		if (Controller)
		{
			if (APawn* NewPawn = Cast<APawn>(NewActor))
				Controller->Possess(NewPawn);
		}
		// 完成延迟构造
		UGameplayStatics::FinishSpawningActor(NewActor, SpawnTransform);
	}
	else
	{
		UE_LOG(LogAction, Error, TEXT("生成Actor失败"));
	}
	

	//加载组件
	LoadActionData(Data, NewActor,OnCompleted);
	
	return NewActor;
}
