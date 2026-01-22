#pragma once

#include "CoreMinimal.h"
#include "CancerCreatorActionBase.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Action_AddActorComponent.generated.h"

UENUM()
enum class ECancerActionType
{
	Character,
	Controller
};


DECLARE_LOG_CATEGORY_EXTERN(LogAction, Log, All);

UCLASS()
class CANCERACTIONCREATOR_API UAction_AddActorComponent : public UCancerCreatorActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component", DisplayName="组件名称")
	FName ComponentNameOverride;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component", DisplayName="组件Class")
	TSubclassOf<UActorComponent> ComponentClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component", DisplayName="目标")
	ECancerActionType CancerActionType{ECancerActionType::Character};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component", DisplayName="组件标签组", meta=(Categories="组件标签"))
	FGameplayTagContainer ComponentTags;

	//获取组件标签组
	UFUNCTION(BlueprintPure)
	TArray<FName> GetComponentTags() const;

	//获取组件标签组最后一个标签名称
	UFUNCTION(BlueprintPure)
	static FName GetGameplayTagLastName(FGameplayTag Tag);

	UFUNCTION()
	UActorComponent* CreateComponent(AActor* InActor);

	virtual void PreExecuteAction_Implementation(AActor* InActor) override;

	//创建组件(将组件挂载到Actor)
	virtual void PostExecuteAction_Implementation(AActor* InActor) override;

	//组件数据初始化
	virtual void PostInitComponent_Implementation() override;

	//(组件注册到Actor)
	virtual void FinalizeAfterComponent_Implementation() override;


	//获取组件
	UFUNCTION(BlueprintPure)
	UActorComponent* GetCreateComponent() const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UActorComponent> WeakComponent;
};
