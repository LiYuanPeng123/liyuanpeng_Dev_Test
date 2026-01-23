#include "CancerActionLibrary.h"
#include "Action_DataAssetTemplate.h"
#include "CancerActionInterface.h"
#include "CancerAssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


DEFINE_LOG_CATEGORY_STATIC(LogAction, Log, All);

void UCancerActionLibrary::LoadActionData(TSoftObjectPtr<UAction_DataAsset> Data, AActor* Actor)
{
	if (Data.IsNull()||!Actor)
		return;
	auto ActionData = UCancerAssetManager::Get().GetAsset(Data);
	TSharedPtr<FStreamableHandle> Handle = ActionData->StartAsyncLoadAndExecuteWithHandle(
		Actor, FSimpleDelegate::CreateLambda(
			[Actor,ActionData]()
			{
				ActionData->PreInitComponent();
				ActionData->PostInitComponent();
				ActionData->FinalizeAfterComponent();
				if (Actor->Implements<UCancerActionInterface>())
				{
					ICancerActionInterface::Execute_HandleOnActionCompleted(Actor);
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
		UE_LOG(LogTemp, Error, TEXT("UCancerActionLibrary::SpawnActorFromActionData PawnData 没有配置"));
		return nullptr;
	}

	UAction_DataAsset* PawnData = UCancerAssetManager::Get().GetAsset(Data);
	if (!PawnData) return nullptr;

	APawn* NewPawn = nullptr;

	TSubclassOf<AAIController> AIControllerClass;
	if (UAction_DataAsset* ActionData = Data.LoadSynchronous())
	{
		if (!ActionData->CustomControllerClass.IsNull())
		{
			AIControllerClass = PawnData->CustomControllerClass.LoadSynchronous();
		}
	}


	// 如果为空 尝试从GameMode加载默认的AIController类
	if (AIControllerClass == nullptr)
	{
		AIControllerClass = AAIController::StaticClass();
	}
	FVector SpawnLocation = SpawnTrans.GetLocation(); // 默认位置
	FRotator SpawnRotation = FRotator(0, SpawnTrans.GetRotation().Rotator().Yaw, 0);

	// 生成 AIController
	FActorSpawnParameters ControllerSpawnParams;
	ControllerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ControllerSpawnParams.Owner = nullptr;
	AAIController* NewAIController = World->SpawnActor<AAIController>(
		AIControllerClass,
		SpawnLocation,
		SpawnRotation,
		ControllerSpawnParams
	);

	if (!NewAIController)return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (auto PawnClass = UCancerAssetManager::Get().GetSubclass(PawnData->PawnClass))
	{
		// 使用延迟构造方式生成Pawn
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);
		NewPawn = World->SpawnActorDeferred<APawn>(PawnClass, SpawnTransform, nullptr,
		                                           nullptr,
		                                           ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		if (NewPawn)
		{
			NewAIController->Possess(NewPawn);
			// 完成延迟构造
			UGameplayStatics::FinishSpawningActor(NewPawn, SpawnTransform);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("生成AI失败"));
		}
	}
	auto HeroData = UCancerAssetManager::GetAsset(Data);
	if (!HeroData)
	{
		UE_LOG(LogTemp, Error, TEXT("PawnData 无法加载"));
		return nullptr;
	}
	//加载依赖资产
	TArray<FSoftObjectPath> Paths;
	HeroData->GatherSoftReferences(Paths);
	if (Paths.Num() > 0)
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(Paths, FStreamableDelegate());
	}

	//加载组件
	LoadActionData(Data, NewPawn);

	return NewPawn;
}

APawn* UCancerActionLibrary::K2_CreatePawnFromActionData(UObject* WorldContextObject, UAction_DataAsset* ActionData,
                                                         const FTransform& SpawnTransform, FOnActionCompled OnCompleted,
                                                         ESpawnActorCollisionHandlingMethod CollisionHandlingMethod,
                                                         AActor* InOwner)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;
	if (!ActionData)
	{
		UE_LOG(LogAction, Error, TEXT("ActionData is null"));
		return nullptr;
	}
	if (ActionData->PawnClass.IsNull())
	{
		UE_LOG(LogAction, Error, TEXT("PawnData中 PawnClass 没有配置"));
		return nullptr;
	}
	if (ActionData->CustomControllerClass.IsNull())
	{
		UE_LOG(LogAction, Error, TEXT("PawnData中 CustomControllerClass 没有配置"));
		return nullptr;
	}


	//生成控制器
	TSubclassOf<AController> ControllerClass;
	ControllerClass = ActionData->CustomControllerClass.LoadSynchronous();
	FVector SpawnLocation = SpawnTransform.GetLocation();
	FRotator SpawnRotation = SpawnTransform.Rotator();

	FActorSpawnParameters ControllerSpawnParams;
	ControllerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ControllerSpawnParams.Owner = InOwner;
	AController* Controller = World->SpawnActor<AController>(
		ControllerClass,
		SpawnLocation,
		SpawnRotation,
		ControllerSpawnParams
	);


	//生成角色
	APawn* OutPawn = nullptr;
	if (auto PawnClass = UCancerAssetManager::Get().GetSubclass(ActionData->PawnClass))
	{
		// 使用延迟构造方式生成Pawn
		OutPawn = World->SpawnActorDeferred<APawn>(PawnClass, SpawnTransform, InOwner,
		                                           nullptr,
		                                           CollisionHandlingMethod);

		Controller->Possess(OutPawn);
		// 完成延迟构造
		UGameplayStatics::FinishSpawningActor(OutPawn, SpawnTransform);
	}

	if (!OutPawn)
	{
		UE_LOG(LogAction, Error, TEXT("生成角色失败"))
		return nullptr;
	}

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
		OutPawn, FSimpleDelegate::CreateLambda([OutPawn,OnCompleted,Controller, ActionData]()
		{
			ActionData->PreInitComponent();
			ActionData->PostInitComponent();
			ActionData->FinalizeAfterComponent();
			if (ACharacter* Character = Cast<ACharacter>(OutPawn))
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
				//广播初始化已经完成
				if (Character->Implements<UCancerActionInterface>())
				{
					ICancerActionInterface::Execute_HandleOnActionCompleted(Character);
				}
				OnCompleted.ExecuteIfBound(OutPawn, Controller);
			}
		}));

	return OutPawn;
}
