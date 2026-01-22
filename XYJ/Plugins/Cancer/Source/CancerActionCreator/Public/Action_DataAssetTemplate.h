#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CancerCharacter.h"
#include "Actions/CancerCreatorActionBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UObject/SoftObjectPath.h"
#include "Delegates/Delegate.h"
#include "Action_DataAssetTemplate.generated.h"

UCLASS()
class CANCERACTIONCREATOR_API UAction_DataAssetTemplate : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName="ActionArray")
	TArray<TObjectPtr<class UCancerCreatorActionBase>> ActionArray;

	void PostExecuteAction(AActor* Actor)
	{
		for (auto Action : ActionArray)
		{
			Action->PostExecuteAction(Actor);
		}
	}

	void PreExecuteAction(AActor* Actor)
	{
		for (auto Action : ActionArray)
		{
			Action->PreExecuteAction(Actor);
		}
	}

	void PreInitComponent()
	{
		for (auto Action : ActionArray)
		{
			Action->PreInitComponent();
		}
	}

	void PostInitComponent()
	{
		for (auto Action : ActionArray)
		{
			Action->PostInitComponent();
		}
	}

	void FinalizeAfterComponent()
	{
		for (auto Action : ActionArray)
		{
			Action->FinalizeAfterComponent();
		}
	}

	void Destroy(AActor* Actor)
	{
		for (auto Action : ActionArray)
		{
			Action->PreDestroyAction(Actor);
			Action->PostDestroyAction(Actor);
		}
	}
};

UCLASS()
class CANCERACTIONCREATOR_API UAction_DataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Pawn, DisplayName="Pawn Class")
	TSoftClassPtr<ACancerCharacter> PawnClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", DisplayName = "AI控制器")
	TSoftClassPtr<AAIController> CustomAIControllerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category= Action, DisplayName="默认Actions")
	TArray<UAction_DataAssetTemplate*> TemplateActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced,Category= Action, DisplayName="ActionArray")
	TArray<TObjectPtr<class UCancerCreatorActionBase>> ActionArray;

	void PostExecute(AActor* Actor)
	{
		for (auto Action : TemplateActions)
		{
			if (Action)
			Action->PostExecuteAction(Actor);
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			Action->PostExecuteAction(Actor);
		}
	}

	void PreExecute(AActor* Actor)
	{
		for (auto TemplateAction : TemplateActions)
		{
			if (TemplateAction)
			TemplateAction->PreExecuteAction(Actor);
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			Action->PreExecuteAction(Actor);
		}
	}

	void PreInitComponent()
	{
		for (auto TemplateAction : TemplateActions)
		{
			if (TemplateAction)
			TemplateAction->PreInitComponent();
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			Action->PreInitComponent();
		}
	}

	void PostInitComponent()
	{
		for (auto TemplateAction : TemplateActions)
		{
			if (TemplateAction)
			TemplateAction->PostInitComponent();
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			Action->PostInitComponent();
		}
	}

	void FinalizeAfterComponent()
	{
		for (auto TemplateAction : TemplateActions)
		{
			if (TemplateAction)
			TemplateAction->FinalizeAfterComponent();
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			Action->FinalizeAfterComponent();
		}
	}


	void Destroy(AActor* Actor)
	{
		for (auto Action : TemplateActions)
		{
			if (Action)
			Action->Destroy(Actor);
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->PreDestroyAction(Actor);
				Action->PostDestroyAction(Actor);
			}
			
		}
	}

	void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
	{
		for (auto Template : TemplateActions)
		{
			for (auto Action : Template->ActionArray)
			{
				Action->GatherSoftReferences(OutPaths);
			}
		}
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->GatherSoftReferences(OutPaths);
			}
		}
	}

	bool StartAsyncLoadAndExecute(AActor* Actor, FSimpleDelegate OnCompleted = FSimpleDelegate())
	{
		TArray<FSoftObjectPath> Paths;
		GatherSoftReferences(Paths);
		if (Paths.Num() == 0)
		{
			PreExecute(Actor);
			PostExecute(Actor);
			if (OnCompleted.IsBound()) OnCompleted.Execute();
			return true;
		}
		TWeakObjectPtr<AActor> WeakActor = Actor;
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(Paths, FStreamableDelegate::CreateLambda(
			                                                                   [WeakActor, this, OnCompleted]() mutable
			                                                                   {
				                                                                   if (!WeakActor.IsValid()) return;
				                                                                   PreExecute(WeakActor.Get());
				                                                                   PostExecute(WeakActor.Get());
				                                                                   if (OnCompleted.IsBound())
					                                                                   OnCompleted.Execute();
			                                                                   }));
		return Handle.IsValid();
	}

	TSharedPtr<FStreamableHandle> StartAsyncLoadAndExecuteWithHandle(AActor* Actor,
	                                                                 FSimpleDelegate OnCompleted = FSimpleDelegate())
	{
		TArray<FSoftObjectPath> Paths;
		GatherSoftReferences(Paths);
		if (Paths.Num() == 0)
		{
			PreExecute(Actor);
			PostExecute(Actor);
			if (OnCompleted.IsBound()) OnCompleted.Execute();
			return nullptr;
		}
		TWeakObjectPtr<AActor> WeakActor = Actor;
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(Paths, FStreamableDelegate::CreateLambda(
			                                                                   [WeakActor, this, OnCompleted]() mutable
			                                                                   {
				                                                                   if (!WeakActor.IsValid()) return;
				                                                                   PreExecute(WeakActor.Get());
				                                                                   PostExecute(WeakActor.Get());
				                                                                   if (OnCompleted.IsBound())
					                                                                   OnCompleted.Execute();
			                                                                   }));
		return Handle;
	}
};
