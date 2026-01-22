#include "CancerActionLibrary.h"
#include "Action_DataAssetTemplate.h"
#include "CancerActionInterface.h"
#include "CancerAssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UCancerActionLibrary::LoadActionData(TSoftObjectPtr<UAction_DataAsset> Data, AActor* Actor)
{
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
	TSoftObjectPtr<UAction_DataAsset> Data ,FTransform SpawnTrans)
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
		if (!ActionData->CustomAIControllerClass.IsNull())
		{
			AIControllerClass = PawnData->CustomAIControllerClass.LoadSynchronous();
		}
	}
	
	
	// 如果为空 尝试从GameMode加载默认的AIController类
	if (AIControllerClass == nullptr)
	{
		AIControllerClass = AAIController ::StaticClass();
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
	LoadActionData(Data,NewPawn);
	
	return NewPawn;
}