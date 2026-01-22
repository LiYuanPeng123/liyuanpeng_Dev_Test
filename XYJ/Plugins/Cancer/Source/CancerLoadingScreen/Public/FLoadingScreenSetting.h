#pragma once


#include "FLoadingScreenSetting.generated.h"

//视频播放类型
UENUM(BlueprintType)
enum class EVideoPlayType:uint8
{
	//一次性播放播放列表中的每部电影
	MT_Normal UMETA(DisplayName="正常播放模式"),
	//按顺序播放播放列表中的所有电影，然后重新开始，直到手动取消为止 
	MT_Looped UMETA(DisplayName = "循环播放模式"),
	//播放播放列表中的所有电影，并循环播放最后一部电影，直到加载完成。
	MT_LoadingLoop UMETA(DisplayName = "备用循环模式"),
};


USTRUCT(BlueprintType)
struct FLoadingScreenSetting
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="UI界面"))
	TSubclassOf<class UUserWidget> WidgetTemplate;
	//路径为Content/Movies
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="影片路径"))
	TArray<FString> MoviePaths;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="影片播放类型"))
	EVideoPlayType PlaybackType = EVideoPlayType::MT_Normal;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="加载完成自动关闭"))
	bool bAutoCompleteWhenLoadingCompletes = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="影片可以跳过"))
	bool bMoviesAreSkippable = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="影片需要主动跳过"))
	bool bWaitForManualStop = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="最小时间"))
	float MinimumLoadingScreenDisplayTime = 5;
};
