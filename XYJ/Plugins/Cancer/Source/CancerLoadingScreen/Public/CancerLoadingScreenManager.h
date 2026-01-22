
#pragma once

#include "CoreMinimal.h"
#include "CancerLoadingScreenManager.generated.h"

UCLASS()
class CANCERLOADINGSCREEN_API UCancerLoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	//~FTickableObjectBase interface
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	//~End of FTickableObjectBase interface
	

	//是否正在加载屏幕
	bool GetLoadingScreenDisplayStatus() const
	{
		return bCurrentlyShowingLoadingScreen;
	}
	
private:
	void HandlePreLoadMap( const FString& MapName);
	void HandlePostLoadMap(UWorld* World);
private:
	/**第一次加载**/
	bool bStartFirstLoadingScreen;

	/** 当我们处于“预加载地图”与“后加载地图”之间时为真 */ 
	bool bCurrentlyInLoadMap = false;

	/** /** 当加载界面正在显示时为真*/
	bool bCurrentlyShowingLoadingScreen = false;
};
