
#include "CancerLoadingScreenManager.h"

#include "CancerLoadingScreenSetting.h"
#include "FLoadingScreenSetting.h"
#include "MoviePlayer.h"
#include "MoviePlayer.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLoadingScreen, Log, All);

void UCancerLoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bStartFirstLoadingScreen = true;
	if (UCancerLoadingScreenSetting::Get()->bEnableLoadingScreen)
	{
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ThisClass::HandlePreLoadMap); //地图开始加载
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ThisClass::HandlePostLoadMap);//地图加载完成
	}
}

void UCancerLoadingScreenManager::Deinitialize()
{
	Super::Deinitialize();
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// We are done, so do not attempt to tick us again
	SetTickableTickType(ETickableTickType::Never);
}

void UCancerLoadingScreenManager::Tick(float DeltaTime)
{

}

ETickableTickType UCancerLoadingScreenManager::GetTickableTickType() const
{
	return FTickableGameObject::GetTickableTickType();
}

bool UCancerLoadingScreenManager::IsTickable() const
{
	return FTickableGameObject::IsTickable();
}

TStatId UCancerLoadingScreenManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCancerLoadingScreenManager, STATGROUP_Tickables);
}

UWorld* UCancerLoadingScreenManager::GetTickableGameObjectWorld() const
{
	return FTickableGameObject::GetTickableGameObjectWorld();
}

void UCancerLoadingScreenManager::HandlePreLoadMap( const FString& MapName)
{
	FLoadingScreenSetting LoadingScreenSetting = bStartFirstLoadingScreen
		                                             ? UCancerLoadingScreenSetting::Get()->StartLoadingScreenSetting
		                                             : UCancerLoadingScreenSetting::Get()->DefaultLoadingScreenSetting;
	bStartFirstLoadingScreen = false;
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.MoviePaths = LoadingScreenSetting.MoviePaths;
	LoadingScreen.MinimumLoadingScreenDisplayTime = LoadingScreenSetting.MinimumLoadingScreenDisplayTime;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = LoadingScreenSetting.bAutoCompleteWhenLoadingCompletes;
	LoadingScreen.bMoviesAreSkippable = LoadingScreenSetting.bMoviesAreSkippable;
	LoadingScreen.bWaitForManualStop = LoadingScreenSetting.bWaitForManualStop;
	LoadingScreen.PlaybackType = (EMoviePlaybackType)LoadingScreenSetting.PlaybackType;
	LoadingScreen.WidgetLoadingScreen = nullptr;
	if (TSubclassOf<class UUserWidget> WidgetTemplate = LoadingScreenSetting.WidgetTemplate)
	{
		if (UUserWidget* LoadingScreenUserWidget = CreateWidget<UUserWidget>(GetOuterUGameInstance(), WidgetTemplate))
		{
			LoadingScreen.WidgetLoadingScreen = LoadingScreenUserWidget->TakeWidget();
		}
	}
	//运行加载屏幕
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UCancerLoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
}
