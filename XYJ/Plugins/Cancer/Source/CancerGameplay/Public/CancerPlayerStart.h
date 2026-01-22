#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CancerPlayerStart.generated.h"


//玩家生成点
UCLASS(MinimalAPI, Config = Game)
class ACancerPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACancerPlayerStart(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
