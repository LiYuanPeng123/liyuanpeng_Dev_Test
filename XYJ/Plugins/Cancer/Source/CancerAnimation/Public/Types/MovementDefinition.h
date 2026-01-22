
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MovementDefinition.generated.h"


class UCancerMovementComponent;

USTRUCT(BlueprintType)
struct FCancer_MovementParameter
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="旋转速率")
	FRotator RotationRate = FRotator(0,360,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="最大移动速度")
	float MaxWalkSpeed = 300;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="最大蹲伏行走速度")
	float MaxWalkSpeedCrouched = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName= "最大加速度")
	float MaxAcceleration  = 2040;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="制动降速行走")
	float BrakingDecelerationWalking = 1500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="地面摩擦力")
	float GroundFriction = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="制动摩擦力因子")
	float BrakingFrictionFactor = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="使用单独的制动摩擦力")
	bool bUseSeparateBrakingFriction = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CancerMovement",DisplayName="制动摩擦力",
		meta=(EditCondition="bUseSeparateBrakingFriction"))
	float BrakingFriction = 1;
};


UCLASS(BlueprintType,Blueprintable,EditInlineNew)
class CANCERANIMATION_API UCancerMovementNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "节点标签")
	FName NodeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Instanced, Category = "子节点")
	TArray<UCancerMovementNode*> ChildrenNode;
	
	virtual void PreUpdateMovementParameter(UCancerMovementComponent* Movement){};
	virtual void PostUpdateMovementParameter(UCancerMovementComponent* Movement){};
	virtual  void UpdateMovementRotation(UCancerMovementComponent* Movement,bool bUseCurveRotation = false){};
	
	UFUNCTION(BlueprintCallable,Meta = (BlueprintThreadSafe))
	virtual float GetMaxWalkSpeed()const ;
	UFUNCTION(BlueprintCallable,Meta = (BlueprintThreadSafe))
	virtual float GetMaxWalkSpeedCrouched()const;

	UFUNCTION(BlueprintCallable,Meta = (BlueprintThreadSafe))
	virtual void SetMaxWalkSpeed(float Speed) ;
	UFUNCTION(BlueprintCallable,Meta = (BlueprintThreadSafe))
	virtual void SetMaxWalkSpeedCrouched(float Speed);
	
	UFUNCTION(BlueprintCallable, Category = "CancerMovement Node",DisplayName="查找Node")
	UCancerMovementNode* FindNodeByName(const FName& InName)
	{
		if (NodeName == InName)
		{
			return const_cast<UCancerMovementNode*>(this);
		}
		
		FString Left;
		FString Right;
		bool IsSucceed = InName.ToString().Split(".", &Left, &Right);
		
		// 检查当前节点是否是目标标签的父标签
		if (!Left.IsEmpty()&& Left == NodeName)
		{
			// 在子节点中查找完全匹配的标签
			for (UCancerMovementNode* Child : ChildrenNode)
			{
				if (Child)
				{
					UCancerMovementNode* FoundNode = Child->FindNodeByName(*Right);
					if (FoundNode)
					{
						return FoundNode;
					}
				}
			}
		}
		return nullptr;
	}
};

UCLASS(BlueprintType,Blueprintable,EditInlineNew)
class CANCERANIMATION_API UCancerMovementNode1 : public UCancerMovementNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "参数")
	FCancer_MovementParameter MovementParameter;
	virtual  void PreUpdateMovementParameter(UCancerMovementComponent* Movement) override;
	virtual  void PostUpdateMovementParameter(UCancerMovementComponent* Movement) override;
	virtual void UpdateMovementRotation(UCancerMovementComponent* Movement, bool bUseCurveRotation = false) override;
	virtual float GetMaxWalkSpeed() const override;
	virtual float GetMaxWalkSpeedCrouched() const override;
	virtual void SetMaxWalkSpeed(float Speed)  override;
	virtual void SetMaxWalkSpeedCrouched(float Speed)  override;
};


