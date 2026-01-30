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
			if (Action)
			{
				Action->PostExecuteAction(Actor);
			}
		}
	}

	void PreExecuteAction(AActor* Actor)
	{
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->PreExecuteAction(Actor);
			}
		}
	}

	void PreInitComponent()
	{
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->PreInitComponent();
			}
		}
	}

	void PostInitComponent()
	{
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->PostInitComponent();
			}
		}
	}

	void FinalizeAfterComponent()
	{
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->FinalizeAfterComponent();
			}
		}
	}

	// 建议使用 Cleanup 替代 Destroy，避免歧义
	void Cleanup(AActor* Actor)
	{
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->PreDestroyAction(Actor);
				Action->PostDestroyAction(Actor);
			}
		}
	}

	UE_DEPRECATED(5.0, "Please use Cleanup() instead to avoid confusion with UObject::Destroy")
	void Destroy(AActor* Actor)
	{
		Cleanup(Actor);
	}

	void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
	{
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->GatherSoftReferences(OutPaths);
			}
		}
	}
};

UCLASS()
class CANCERACTIONCREATOR_API UAction_DataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//Actor的唯一标识 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Action, DisplayName="ActorTag")
	FGameplayTag ActorTag;

	//Actor附带查询规则 (势力,属性等)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Action, DisplayName="ActorTagContainer")
	FGameplayTagContainer ActorTagContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Action, DisplayName="ActorClass")
	TSubclassOf<AActor> ActorClass = AActor::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, DisplayName = "Controller",
		meta = (EditCondition = "bIsPawn", EditConditionHides))
	TSubclassOf<AController> ControllerClass = AController::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Action, DisplayName="默认Actions")
	TArray<UAction_DataAssetTemplate*> TemplateActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category= Action, DisplayName="ActionArray")
	TArray<TObjectPtr<class UCancerCreatorActionBase>> ActionArray;

	void PostExecute(AActor* Actor)
	{
		ForEachAction([Actor](auto* Action) { Action->PostExecuteAction(Actor); });
	}

	void PreExecute(AActor* Actor)
	{
		ForEachAction([Actor](auto* Action) { Action->PreExecuteAction(Actor); });
	}

	void PreInitComponent()
	{
		ForEachAction([](auto* Action) { Action->PreInitComponent(); });
	}

	void PostInitComponent()
	{
		ForEachAction([](auto* Action) { Action->PostInitComponent(); });
	}

	void FinalizeAfterComponent()
	{
		ForEachAction([](auto* Action) { Action->FinalizeAfterComponent(); });
	}

	void Cleanup(AActor* Actor)
	{
		// Template 使用 Cleanup
		for (auto Template : TemplateActions)
		{
			if (Template) Template->Cleanup(Actor);
		}
		// Action 使用 Pre/Post Destroy
		for (auto Action : ActionArray)
		{
			if (Action)
			{
				Action->PreDestroyAction(Actor);
				Action->PostDestroyAction(Actor);
			}
		}
	}

	UE_DEPRECATED(5.0, "Please use Cleanup() instead")
	void Destroy(AActor* Actor)
	{
		Cleanup(Actor);
	}

	void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
	{
		//  收集 Actions 的软引用
		ForEachAction([&OutPaths](const auto* Action)
		{
			if (Action)
			{
				Action->GatherSoftReferences(OutPaths);
			}
		});
	}

	bool StartAsyncLoadAndExecute(AActor* Actor, FSimpleDelegate OnCompleted = FSimpleDelegate())
	{
		return StartAsyncLoadAndExecuteWithHandle(Actor, OnCompleted).IsValid();
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
		TWeakObjectPtr<UAction_DataAsset> WeakThis(this);

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(Paths, FStreamableDelegate::CreateLambda(
			                                                                   [WeakActor, WeakThis, OnCompleted]()
			                                                                   {
				                                                                   if (!WeakActor.IsValid() || !WeakThis
					                                                                   .IsValid())
					                                                                   return;
				                                                                   WeakThis->PreExecute(
					                                                                   WeakActor.Get());
				                                                                   WeakThis->PostExecute(
					                                                                   WeakActor.Get());
				                                                                   if (OnCompleted.IsBound())
					                                                                   OnCompleted.Execute();
			                                                                   }));
		return Handle;
	}

	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "Config", meta = (HideInDetailPanel))
	bool bIsPawn = false;

	// 模板辅助函数，消除重复遍历逻辑
	// Func 接受 (UAction_DataAssetTemplate* 或 UCancerCreatorActionBase*)
	template <typename FuncType>
	void ForEachAction(FuncType Func)const
	{
		for (auto TemplateAction : TemplateActions)
		{
			if (TemplateAction) Func(TemplateAction);
		}
		for (const auto& Action : ActionArray)
		{
			if (Action) Func(Action.Get());
		}
	}
};
