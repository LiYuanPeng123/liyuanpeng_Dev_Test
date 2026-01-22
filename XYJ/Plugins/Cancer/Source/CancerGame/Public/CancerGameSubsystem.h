
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/DataTable.h"
#include "CancerGameSubsystem.generated.h"

/**
 * 负责加载游戏中的一些配置资产
 */
UCLASS()
class CANCERGAME_API UCancerGameSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** 获取文本描述 */
	UFUNCTION(BlueprintCallable, Category = "Game|Settings")
	FString GetTextDescription(int32 ID);

	/** 获取图片资源 */
	UFUNCTION(BlueprintCallable, Category = "Game|Settings")
	TSoftObjectPtr<UTexture2D> GetImage(int32 ID);

protected:
	/** 加载所有数据表 */
	void LoadAllDataTables();

	/** 加载单个数据表 */
	void LoadDataTable(UDataTable* DataTable, bool bIsTextTable);

private:
	/** 存储文本描述的映射 */
	UPROPERTY(Transient)
	TMap<int32, FString> TextDescriptions;

	/** 存储图片资源的映射 */
	UPROPERTY(Transient)
	TMap<int32, TSoftObjectPtr<UTexture2D>> GameImages;
};
