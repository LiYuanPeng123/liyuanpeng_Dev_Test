#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "CancerCharacter.h"
#include "CancerCharacterSubsystem.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Pawn, DisplayName="Pawn Class")
	TSoftClassPtr<ACancerCharacter> PawnClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", DisplayName = "AI控制器")
	TSoftClassPtr<AAIController> CustomControllerClass;
	
	//身份证
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Action, DisplayName="身份证")
	FGameplayTagContainer Identify;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Action, DisplayName="默认Actions")
	TArray<UAction_DataAssetTemplate*> TemplateActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category= Action, DisplayName="ActionArray")
	TArray<TObjectPtr<class UCancerCreatorActionBase>> ActionArray;
	
	void PostExecute(AActor* Actor)
	{
		ForEachAction([Actor](auto* Action) { Action->PostExecuteAction(Actor); });
		
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
		{
			ASC->AddLooseGameplayTags(Identify);
		}
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
		// 1. 收集自身的软引用
		if (!PawnClass.IsNull())
		{
			OutPaths.Add(PawnClass.ToSoftObjectPath());
		}
		if (!CustomControllerClass.IsNull())
		{
			OutPaths.Add(CustomControllerClass.ToSoftObjectPath());
		}

		// 2. 收集 Actions 的软引用
		for (auto Template : TemplateActions)
		{
			if (Template)
			{
				Template->GatherSoftReferences(OutPaths);
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
		
		if (UWorld* World = Actor->GetWorld())
		{
			UCancerCharacterSubsystem* CharacterSubsystem =  World->GetSubsystem<UCancerCharacterSubsystem>();
			if (CharacterSubsystem)
			{
				CharacterSubsystem->AddCharacter(Actor);
			}
		}
		
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(Paths, FStreamableDelegate::CreateLambda(
			                                                                   [WeakActor, WeakThis, OnCompleted]()
			                                                                   {
				                                                                   if (!WeakActor.IsValid() || !WeakThis.IsValid()) return;
				                                                                   WeakThis->PreExecute(WeakActor.Get());
				                                                                   WeakThis->PostExecute(WeakActor.Get());
				                                                                   if (OnCompleted.IsBound())
					                                                                   OnCompleted.Execute();
			                                                                   }));
		return Handle;
	}

private:
	// 模板辅助函数，消除重复遍历逻辑
	// Func 接受 (UAction_DataAssetTemplate* 或 UCancerCreatorActionBase*)
	template<typename FuncType>
	void ForEachAction(FuncType Func)
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
