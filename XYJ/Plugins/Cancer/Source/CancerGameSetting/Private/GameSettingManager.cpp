#include "GameSettingManager.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

UGameSettingManager::UGameSettingManager()
{
	InitializeResolutions();
}

UGameSettingManager* UGameSettingManager::Get()
{
	return GetMutableDefault<UGameSettingManager>();
}

void UGameSettingManager::InitializeResolutions()
{
	AvailableResolutions.Empty();
	AvailableResolutions.Add(FIntPoint(1366, 768));
	AvailableResolutions.Add(FIntPoint(1600, 900));
	AvailableResolutions.Add(FIntPoint(1920, 1080));
	AvailableResolutions.Add(FIntPoint(2560, 1440));
	AvailableResolutions.Add(FIntPoint(3840, 2160));
}

// ========== 音频设置实现 ==========
void UGameSettingManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	// 使用控制台命令设置音量
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("au.GlobalVolumeMultiplier %f"), MasterVolume);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("主音量设置为: %.2f"), MasterVolume);
}

void UGameSettingManager::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("音效音量设置为: %.2f"), SFXVolume);
}

void UGameSettingManager::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("音乐音量设置为: %.2f"), MusicVolume);
}

void UGameSettingManager::SetVoiceVolume(float Volume)
{
	VoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("语音音量设置为: %.2f"), VoiceVolume);
}

void UGameSettingManager::SetAudioQualityIndex(int32 Index)
{
	AudioQualityIndex = FMath::Clamp(Index, 0, 3);

	// 根据音频质量设置相应的音频参数
	if (GEngine)
	{
		FString Command;
		switch (AudioQualityIndex)
		{
		case 0: // 低质量
			Command = TEXT("au.MaxChannels 32");
			break;
		case 1: // 中质量
			Command = TEXT("au.MaxChannels 64");
			break;
		case 2: // 高质量
			Command = TEXT("au.MaxChannels 128");
			break;
		case 3: // 极高质量
			Command = TEXT("au.MaxChannels 256");
			break;
		}
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("音频质量设置为: %d"), AudioQualityIndex);
}

void UGameSettingManager::SetMuteEnabled(bool bEnabled)
{
	bMuteEnabled = bEnabled;

	// 应用静音设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("au.GlobalVolumeMultiplier %f"),
										  bMuteEnabled ? 0.0f : MasterVolume);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("静音设置为: %s"), bMuteEnabled ? TEXT("开启") : TEXT("关闭"));
}


// ========== 图形设置实现 ==========

void UGameSettingManager::SetViewDistanceIndex(int32 Index)
{
	ViewDistanceIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetViewDistanceQuality(ViewDistanceIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("视野距离设置为: %d"), ViewDistanceIndex);
}

void UGameSettingManager::SetAntiAliasingIndex(int32 Index)
{
	AntiAliasingIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetAntiAliasingQuality(AntiAliasingIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("抗锯齿设置为: %d"), AntiAliasingIndex);
}

void UGameSettingManager::SetPostProcessQualityIndex(int32 Index)
{
	PostProcessQualityIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetPostProcessingQuality(PostProcessQualityIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("后期处理质量设置为: %d"), PostProcessQualityIndex);
}

void UGameSettingManager::SetShadowQualityIndex(int32 Index)
{
	ShadowQualityIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetShadowQuality(ShadowQualityIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("阴影质量设置为: %d"), ShadowQualityIndex);
}

void UGameSettingManager::SetTextureQualityIndex(int32 Index)
{
	TextureQualityIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetTextureQuality(TextureQualityIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("贴图质量设置为: %d"), TextureQualityIndex);
}

void UGameSettingManager::SetEffectsQualityIndex(int32 Index)
{
	EffectsQualityIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetVisualEffectQuality(EffectsQualityIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("特效质量设置为: %d"), EffectsQualityIndex);
}

void UGameSettingManager::SetFoliageQualityIndex(int32 Index)
{
	FoliageQualityIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetFoliageQuality(FoliageQualityIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("植被质量设置为: %d"), FoliageQualityIndex);
}

void UGameSettingManager::SetShadingQualityIndex(int32 Index)
{
	ShadingQualityIndex = FMath::Clamp(Index, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetShadingQuality(ShadingQualityIndex);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("着色器质量设置为: %d"), ShadingQualityIndex);
}

void UGameSettingManager::SetGraphicsQuality(int32 Quality)
{
	GraphicsQuality = FMath::Clamp(Quality, 0, 3);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetOverallScalabilityLevel(GraphicsQuality);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("图形质量设置为: %d"), GraphicsQuality);
}

void UGameSettingManager::SetResolutionIndex(int32 Index)
{
	if (AvailableResolutions.IsValidIndex(Index))
	{
		ResolutionIndex = Index;

		if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
		{
			GameSettings->SetScreenResolution(AvailableResolutions[Index]);
			GameSettings->ApplyResolutionSettings(false);
		}

		SaveConfig();
		UE_LOG(LogTemp, Log, TEXT("分辨率设置为: %dx%d"),
		       AvailableResolutions[Index].X, AvailableResolutions[Index].Y);
	}
}

void UGameSettingManager::SetWindowModeIndex(int32 Index)
{
	WindowModeIndex = FMath::Clamp(Index, 0, 2);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		EWindowMode::Type WindowMode = EWindowMode::Windowed;
		switch (WindowModeIndex)
		{
		case 0: WindowMode = EWindowMode::Windowed;
			break;
		case 1: WindowMode = EWindowMode::WindowedFullscreen;
			break;
		case 2: WindowMode = EWindowMode::Fullscreen;
			break;
		}

		GameSettings->SetFullscreenMode(WindowMode);
		GameSettings->ApplyResolutionSettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("窗口模式设置为: %d"), WindowModeIndex);
}

void UGameSettingManager::SetVSyncIndex(int32 Index)
{
	VSyncIndex = FMath::Clamp(Index, 0, 1);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetVSyncEnabled(VSyncIndex == 1);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("垂直同步设置为: %d"), VSyncIndex);
}

void UGameSettingManager::SetFrameRateLimit(int32 Limit)
{
	FrameRateLimit = FMath::Clamp(Limit, 30.0f, 144.0f);

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetFrameRateLimit(FrameRateLimit);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("帧率限制设置为: %d"), FrameRateLimit);
}

void UGameSettingManager::SetMouseSensitivity(float Sensitivity)
{
	MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("鼠标灵敏度设置为: %.2f"), MouseSensitivity);
}

void UGameSettingManager::SetInvertMouseY(bool bEnabled)
{
	InvertMouseYIndex = bEnabled ? 1 : 0;
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("反转鼠标Y轴设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetControllerVibrationEnabled(bool bEnabled)
{
	bControllerVibrationEnabled = bEnabled;
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("手柄震动设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetControllerSensitivity(float Sensitivity)
{
	ControllerSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("手柄灵敏度设置为: %.2f"), ControllerSensitivity);
}

void UGameSettingManager::SetDifficultyIndex(int32 Index)
{
	DifficultyLevel = FMath::Clamp(Index, 0, 3);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("难度等级设置为: %d"), DifficultyLevel);
}

void UGameSettingManager::SetSubtitlesEnabled(bool bEnabled)
{
	SubtitlesIndex = bEnabled ? 1 : 0;
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("字幕设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetColorBlindAssistIndex(int32 Index)
{
	ColorBlindFriendlyIndex = FMath::Clamp(Index, 0, 3);

	// 应用色盲辅助设置
	if (GEngine)
	{
		FString Command;
		switch (ColorBlindFriendlyIndex)
		{
		case 0: // 关闭
			Command = TEXT("r.Color.Deficiency.Type 0");
			break;
		case 1: // 红绿色盲
			Command = TEXT("r.Color.Deficiency.Type 1");
			break;
		case 2: // 蓝黄色盲
			Command = TEXT("r.Color.Deficiency.Type 2");
			break;
		case 3: // 全色盲
			Command = TEXT("r.Color.Deficiency.Type 3");
			break;
		}
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("色盲辅助设置为: %d"), ColorBlindFriendlyIndex);
}

void UGameSettingManager::SetSubtitleSize(float Size)
{
	FontSize = FMath::Clamp(Size, 0.5f, 2.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("字幕大小设置为: %.2f"), FontSize);
}

void UGameSettingManager::SetShowFPSEnabled(bool bEnabled)
{
	ShowDebugInfoIndex = bEnabled ? 1 : 0;

	// 应用FPS显示设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("stat fps %d"), ShowDebugInfoIndex);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("显示FPS设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetDebugInfoEnabled(bool bEnabled)
{
	ShowDebugInfoIndex = bEnabled ? 1 : 0;

	// 应用调试信息显示设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("stat unit %d"), ShowDebugInfoIndex);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("显示调试信息设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetLogLevelIndex(int32 Index)
{
	LogLevelIndex = FMath::Clamp(Index, 0, 4);

	// 应用日志级别设置
	if (GEngine)
	{
		FString Command;
		switch (LogLevelIndex)
		{
		case 0: // 关闭
			Command = TEXT("log LogTemp off");
			break;
		case 1: // 错误
			Command = TEXT("log LogTemp Error");
			break;
		case 2: // 警告
			Command = TEXT("log LogTemp Warning");
			break;
		case 3: // 信息
			Command = TEXT("log LogTemp Log");
			break;
		case 4: // 详细
			Command = TEXT("log LogTemp Verbose");
			break;
		}
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("日志级别设置为: %d"), LogLevelIndex);
}

void UGameSettingManager::SetPerformanceStatsEnabled(bool bEnabled)
{
	bPerformanceStatsEnabled = bEnabled;

	// 应用性能统计设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("stat game %d"), bEnabled ? 1 : 0);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("性能统计设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetDeveloperModeEnabled(bool bEnabled)
{
	bDeveloperModeEnabled = bEnabled;

	// 应用开发者模式设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("showdebug %s"), bEnabled ? TEXT("all") : TEXT("none"));
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("开发者模式设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetInvertMouseYIndex(int32 Index)
{
	InvertMouseYIndex = FMath::Clamp(Index, 0, 1);

	// 移除错误的SetMouseSensitivity调用
	// 鼠标Y轴反转设置通常在游戏的输入处理中实现，而不是在UGameUserSettings中
	// 如果需要应用设置，可以通过游戏的输入管理器或PlayerController来处理

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("鼠标Y轴反转设置为: %d"), InvertMouseYIndex);
}

void UGameSettingManager::SetDifficultyLevel(int32 Level)
{
	DifficultyLevel = FMath::Clamp(Level, 0, 3);

	// 这里可以添加难度级别的具体应用逻辑
	// 例如：调整敌人血量、伤害倍数等

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("难度级别设置为: %d"), DifficultyLevel);
}

void UGameSettingManager::SetBrightness(float Value)
{
	Brightness = FMath::Clamp(Value, 0.0f, 2.0f);

	// 应用亮度设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("r.Color.Brightness %f"), Brightness);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("亮度设置为: %.2f"), Brightness);
}

void UGameSettingManager::SetContrast(float Value)
{
	Contrast = FMath::Clamp(Value, 0.0f, 2.0f);

	// 应用对比度设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("r.Color.Contrast %f"), Contrast);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("对比度设置为: %.2f"), Contrast);
}

void UGameSettingManager::SetFrameRateLimitIndex(int32 Index)
{
	FrameRateLimitIndex = FMath::Clamp(Index, 0, 5);

	// 根据索引设置帧率限制
	int32 Limits[] = {0, 30, 60, 90, 120, 144};
	if (Index < UE_ARRAY_COUNT(Limits))
	{
		SetFrameRateLimit(Limits[Index]);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("帧率限制索引设置为: %d"), FrameRateLimitIndex);
}

void UGameSettingManager::SetVSyncEnabled(bool bEnabled)
{
	VSyncIndex = bEnabled ? 1 : 0;

	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetVSyncEnabled(bEnabled);
		GameSettings->ApplySettings(false);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("垂直同步设置为: %s"), bEnabled ? TEXT("开启") : TEXT("关闭"));
}

void UGameSettingManager::SetAutoSaveIndex(int32 Index)
{
	AutoSaveIndex = FMath::Clamp(Index, 0, 3);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("自动保存设置为: %d"), AutoSaveIndex);
}

void UGameSettingManager::SetSubtitlesIndex(int32 Index)
{
	SubtitlesIndex = FMath::Clamp(Index, 0, 1);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("字幕设置为: %d"), SubtitlesIndex);
}

void UGameSettingManager::SetUIScale(float Scale)
{
	UIScale = FMath::Clamp(Scale, 0.5f, 2.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("UI缩放设置为: %.2f"), UIScale);
}

// ========== 辅助功能设置实现 ==========
void UGameSettingManager::SetColorBlindFriendlyIndex(int32 Index)
{
	ColorBlindFriendlyIndex = FMath::Clamp(Index, 0, 3);

	// 应用色盲辅助设置
	if (GEngine)
	{
		FString Command;
		switch (ColorBlindFriendlyIndex)
		{
		case 0: // 关闭
			Command = TEXT("r.Color.Deficiency.Type 0");
			break;
		case 1: // 红绿色盲
			Command = TEXT("r.Color.Deficiency.Type 1");
			break;
		case 2: // 蓝黄色盲
			Command = TEXT("r.Color.Deficiency.Type 2");
			break;
		case 3: // 全色盲
			Command = TEXT("r.Color.Deficiency.Type 3");
			break;
		}
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("色盲辅助设置为: %d"), ColorBlindFriendlyIndex);
}

void UGameSettingManager::SetFontSize(float Size)
{
	FontSize = FMath::Clamp(Size, 0.5f, 2.0f);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("字体大小设置为: %.2f"), FontSize);
}

void UGameSettingManager::SetKeyPromptsIndex(int32 Index)
{
	KeyPromptsIndex = FMath::Clamp(Index, 0, 3);
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("按键提示设置为: %d"), KeyPromptsIndex);
}

void UGameSettingManager::SetHighContrastEnabled(bool bEnabled)
{
	bHighContrastEnabled = bEnabled;

	// 应用高对比度设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("r.Color.Contrast %f"),
		                                  bHighContrastEnabled ? 2.0f : Contrast);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("高对比度模式设置为: %s"), bHighContrastEnabled ? TEXT("开启") : TEXT("关闭"));
}

// ========== 高级设置实现 ==========
void UGameSettingManager::SetShowDebugInfoIndex(int32 Index)
{
	ShowDebugInfoIndex = FMath::Clamp(Index, 0, 1);

	// 应用调试信息显示设置
	if (GEngine)
	{
		FString Command = FString::Printf(TEXT("stat fps %d"), ShowDebugInfoIndex);
		GEngine->Exec(nullptr, *Command);
	}

	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("显示调试信息设置为: %d"), ShowDebugInfoIndex);
}




void UGameSettingManager::ApplyGraphicsPreset(int32 PresetLevel)
{
	int32 Level = FMath::Clamp(PresetLevel, 0, 3);

	UE_LOG(LogTemp, Log, TEXT("应用画质预设: %d"), Level);

	// 应用所有图形设置
	SetGraphicsQuality(Level);
	SetViewDistanceIndex(Level);
	SetAntiAliasingIndex(Level);
	SetPostProcessQualityIndex(Level);
	SetShadowQualityIndex(Level);
	SetTextureQualityIndex(Level);
	SetEffectsQualityIndex(Level);
	SetFoliageQualityIndex(Level);
	SetShadingQualityIndex(Level);

	// 保存配置
	SaveConfig();

	UE_LOG(LogTemp, Log, TEXT("画质预设 %d 应用完成"), Level);
}
