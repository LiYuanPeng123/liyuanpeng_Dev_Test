#pragma once

#include "CoreMinimal.h"
#include "CancerAITypes.h"
#include "Components/ActorComponent.h"
#include "AIPatrolComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CANCERAI_API UAIPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIPatrolComponent();

protected:
	virtual void BeginPlay() override;
	//跟随路径
	UPROPERTY(EditAnywhere,BlueprintReadWrite,
		Category = Cancer)
	class APatrolPath* PathToFollow;
public:
	//获取路径
	UFUNCTION(BlueprintCallable, Category = Cancer)
	class APatrolPath* GetPathToFollow() const { return PathToFollow; }

	//设置路径去跟随
	UFUNCTION(BlueprintCallable, Category = Cancer)
	void SetPathToFollow(class APatrolPath* val) { PathToFollow = val; }

	//尝试获取下一个路径点
	UFUNCTION(BlueprintCallable, Category = Cancer)
	bool TryGetNextWaypoint(FVector& outLocation);

private:
	int32 PatrolIndex = 0;
};
