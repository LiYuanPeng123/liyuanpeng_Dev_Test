#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "GameSettingManager.generated.h"

/**
 * 游戏设置管理器
 * 位于插件内部，管理所有游戏设置
 * 提供统一的设置接口，支持音频、图形、控制、游戏性、辅助功能和高级设置
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Game Setting Manager"))
class CANCERGAMESETTING_API UGameSettingManager : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGameSettingManager();

	/**
	 * 获取游戏设置管理器单例
	 * @return 游戏设置管理器实例
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Settings")
	static UGameSettingManager* Get();

	// ========================================
	// 音频设置 (Audio Settings)
	// ========================================
#pragma region 

	/**
	 * 设置主音量
	 * @param Volume 音量值 (0.0-1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMasterVolume(float Volume);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetMasterVolume() const { return MasterVolume; }

	/**
	 * 设置音效音量
	 * @param Volume 音量值 (0.0-1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSFXVolume(float Volume);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetSFXVolume() const { return SFXVolume; }
	
	/**
    * 设置音乐音量
    * @param Volume 音量值 (0.0-1.0)
    */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMusicVolume(float Volume);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetMusicVolume() const { return MusicVolume; }
	
	/**
	 * 设置语音音量
	 * @param Volume 音量值 (0.0-1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetVoiceVolume(float Volume);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	float GetVoiceVolume() const { return VoiceVolume; }
	
	/** 获取音频质量索引 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	int32 GetAudioQualityIndex() const { return AudioQualityIndex; }

	/**
	 * 设置音频质量
	 * @param Index 质量索引 (0=低, 1=中, 2=高, 3=极高)
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetAudioQualityIndex(int32 Index);

	/**
	 * 设置静音状态
	 * @param bEnabled 是否启用静音
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMuteEnabled(bool bEnabled);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	bool GetMuteEnabled() const { return bMuteEnabled; }

#pragma endregion 

	// ========================================
	// 图形设置 (Graphics Settings)
	// ========================================

#pragma region 

	/**
    * 设置整体图形质量
    * @param Quality 质量等级 (0=低, 1=中, 2=高, 3=史诗)
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetGraphicsQuality(int32 Quality);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetGraphicsQuality() const { return GraphicsQuality; }
	
	/**
	 * 设置分辨率
	 * @param Index 分辨率索引 (对应可用分辨率列表)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetResolutionIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetResolutionIndex() const { return ResolutionIndex; }


	/**
	 * 设置窗口模式
	 * @param Index 窗口模式索引 (0=窗口, 1=无边框窗口, 2=全屏)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetWindowModeIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetWindowModeIndex() const { return WindowModeIndex; }



	/**
    * 设置垂直同步
    * @param Index 垂直同步索引 (0=关闭, 1=开启)
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetVSyncIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetVSyncIndex() const { return VSyncIndex; }
	
	/**
    * 设置垂直同步状态
    * @param bEnabled 是否启用垂直同步
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetVSyncEnabled(bool bEnabled);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	bool GetVSyncEnabled() const { return VSyncIndex == 1; }
	
	/**
	 * 设置帧率限制
	 * @param Limit 帧率限制值 (0=无限制)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetFrameRateLimit(int32 Limit);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetFrameRateLimit() const { return FrameRateLimit; }

	/**
	 * 设置帧率限制索引
	 * @param Index 帧率限制索引 (0=无限制, 1=30fps, 2=60fps, 3=90fps, 4=120fps, 5=144fps)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetFrameRateLimitIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetFrameRateLimitIndex() const { return FrameRateLimitIndex; }



	/**
    * 设置亮度
    * @param Value 亮度值 (0.0-2.0)
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetBrightness(float Value);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	float GetBrightness() const { return Brightness; }



	/** 获取对比度 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	float GetContrast() const { return Contrast; }

	/**
	 * 设置对比度
	 * @param Value 对比度值 (0.0-2.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetContrast(float Value);

	/**
	 * 设置视野距离质量
	 * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetViewDistanceIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetViewDistanceIndex() const { return ViewDistanceIndex; }
	
	/**
    * 设置抗锯齿质量
    * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetAntiAliasingIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetAntiAliasingIndex() const { return AntiAliasingIndex; }
	
	/**
    * 设置后期处理质量
    * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetPostProcessQualityIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetPostProcessQualityIndex() const { return PostProcessQualityIndex; }
	
	/**
    * 设置阴影质量
    * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
    */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetShadowQualityIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetShadowQualityIndex() const { return ShadowQualityIndex; }
	
	/**
	 * 设置贴图质量
	 * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetTextureQualityIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetTextureQualityIndex() const { return TextureQualityIndex; }
	
	/**
	 * 设置特效质量
	 * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetEffectsQualityIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetEffectsQualityIndex() const { return EffectsQualityIndex; }
	
	/**
	 * 设置植被质量
	 * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetFoliageQualityIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetFoliageQualityIndex() const { return FoliageQualityIndex; }
	
	/**
	 * 设置着色器质量
	 * @param Index 质量索引 (0=低, 1=中, 2=高, 3=史诗)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void SetShadingQualityIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	int32 GetShadingQualityIndex() const { return ShadingQualityIndex; }

	
	/**
	 * 应用画质预设
	 * @param PresetLevel 预设等级 (0=低, 1=中, 2=高, 3=史诗)
	 */
	UFUNCTION(BlueprintCallable, Category = "Graphics")
	void ApplyGraphicsPreset(int32 PresetLevel);
#pragma endregion

	// ========================================
	// 控制设置 (Control Settings)
	// ========================================

#pragma region 

	/**
	 * 设置鼠标灵敏度
	 * @param Sensitivity 灵敏度值 (0.1-5.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void SetMouseSensitivity(float Sensitivity);
	UFUNCTION(BlueprintCallable, Category = "Controls")
	float GetMouseSensitivity() const { return MouseSensitivity; }
	
	/** 获取鼠标Y轴反转状态 */
	UFUNCTION(BlueprintPure, Category = "Controls")
	bool GetInvertMouseY() const { return InvertMouseYIndex == 1; }

	/** 获取鼠标Y轴反转索引 */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	int32 GetInvertMouseYIndex() const { return InvertMouseYIndex; }

	/**
	 * 设置鼠标Y轴反转
	 * @param bEnabled 是否反转Y轴
	 */
	void SetInvertMouseY(bool bEnabled);

	/**
	 * 设置鼠标Y轴反转索引
	 * @param Index 反转索引 (0=正常, 1=反转)
	 */
	UFUNCTION(BlueprintCallable, Category = "Controls")
	void SetInvertMouseYIndex(int32 Index);

	/**
	 * 设置手柄震动
	 * @param bEnabled 是否启用手柄震动
	 */
	void SetControllerVibrationEnabled(bool bEnabled);
	UFUNCTION(BlueprintPure, Category = "Controls")
	bool GetControllerVibrationEnabled() const { return bControllerVibrationEnabled; }


	/**
	 * 设置手柄灵敏度
	 * @param Sensitivity 灵敏度值 (0.1-5.0)
	 */
	void SetControllerSensitivity(float Sensitivity);
	UFUNCTION(BlueprintPure, Category = "Controls")
	float GetControllerSensitivity() const { return ControllerSensitivity; }

#pragma endregion 

	// ========================================
	// 游戏性设置 (Gameplay Settings)
	// ========================================
#pragma region 
	/**
    * 设置难度等级
    * @param Level 难度等级 (0=简单, 1=普通, 2=困难, 3=地狱)
    */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetDifficultyLevel(int32 Level);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	int32 GetDifficultyLevel() const { return DifficultyLevel; }
	
	/**
	 * 设置难度索引
	 * @param Index 难度索引 (0=简单, 1=普通, 2=困难, 3=地狱)
	 */
	void SetDifficultyIndex(int32 Index);
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	int32 GetDifficultyIndex() const { return DifficultyLevel; }



	/** 获取自动保存索引 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	int32 GetAutoSaveIndex() const { return AutoSaveIndex; }

	/**
	 * 设置自动保存频率
	 * @param Index 自动保存索引 (0=关闭, 1=每5分钟, 2=每10分钟, 3=每15分钟)
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetAutoSaveIndex(int32 Index);

	/** 获取字幕状态 */
	UFUNCTION(BlueprintPure, Category = "Gameplay")
	bool GetSubtitlesEnabled() const { return SubtitlesIndex == 1; }

	/** 获取字幕索引 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	int32 GetSubtitlesIndex() const { return SubtitlesIndex; }

	/**
	 * 设置字幕显示
	 * @param bEnabled 是否显示字幕
	 */
	void SetSubtitlesEnabled(bool bEnabled);

	/**
	 * 设置字幕索引
	 * @param Index 字幕索引 (0=关闭, 1=开启)
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetSubtitlesIndex(int32 Index);

	/** 获取UI缩放 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	float GetUIScale() const { return UIScale; }

	/**
	 * 设置UI缩放
	 * @param Scale UI缩放值 (0.5-2.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetUIScale(float Scale);

#pragma endregion 
	// ========================================
	// 辅助功能设置 (Accessibility Settings)
	// ========================================

#pragma region 
	/** 获取色盲辅助索引 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	int32 GetColorBlindFriendlyIndex() const { return ColorBlindFriendlyIndex; }

	/** 获取色盲辅助索引（兼容性） */
	UFUNCTION(BlueprintPure, Category = "Accessibility")
	int32 GetColorBlindAssistIndex() const { return ColorBlindFriendlyIndex; }

	/**
	 * 设置色盲辅助
	 * @param Index 色盲辅助索引 (0=关闭, 1=红绿色盲, 2=蓝黄色盲, 3=全色盲)
	 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	void SetColorBlindFriendlyIndex(int32 Index);

	/**
	 * 设置色盲辅助（兼容性）
	 * @param Index 色盲辅助索引 (0=关闭, 1=红绿色盲, 2=蓝黄色盲, 3=全色盲)
	 */
	void SetColorBlindAssistIndex(int32 Index);

	/** 获取字体大小 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	float GetFontSize() const { return FontSize; }

	/** 获取字幕大小（兼容性） */
	UFUNCTION(BlueprintPure, Category = "Accessibility")
	float GetSubtitleSize() const { return FontSize; }

	/**
	 * 设置字体大小
	 * @param Size 字体大小 (0.5-2.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	void SetFontSize(float Size);

	/**
	 * 设置字幕大小（兼容性）
	 * @param Size 字幕大小 (0.5-2.0)
	 */
	void SetSubtitleSize(float Size);

	/** 获取按键提示索引 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	int32 GetKeyPromptsIndex() const { return KeyPromptsIndex; }

	/**
	 * 设置按键提示样式
	 * @param Index 按键提示索引 (0=关闭, 1=键盘, 2=手柄, 3=自动)
	 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	void SetKeyPromptsIndex(int32 Index);

	/** 获取高对比度状态 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	bool GetHighContrastEnabled() const { return bHighContrastEnabled; }

	/**
	 * 设置高对比度模式
	 * @param bEnabled 是否启用高对比度
	 */
	UFUNCTION(BlueprintCallable, Category = "Accessibility")
	void SetHighContrastEnabled(bool bEnabled);

#pragma endregion 
	// ========================================
	// 高级设置 (Advanced Settings)
	// ========================================

#pragma region 
	/** 获取调试信息显示索引 */
	UFUNCTION(BlueprintCallable, Category = "Advanced")
	int32 GetShowDebugInfoIndex() const { return ShowDebugInfoIndex; }

	/** 获取FPS显示状态 */
	UFUNCTION(BlueprintPure, Category = "Advanced")
	bool GetShowFPSEnabled() const { return ShowDebugInfoIndex == 1; }

	/** 获取调试信息状态 */
	UFUNCTION(BlueprintPure, Category = "Advanced")
	bool GetDebugInfoEnabled() const { return ShowDebugInfoIndex == 1; }

	/**
	 * 设置调试信息显示
	 * @param Index 显示索引 (0=关闭, 1=开启)
	 */
	UFUNCTION(BlueprintCallable, Category = "Advanced")
	void SetShowDebugInfoIndex(int32 Index);

	/**
	 * 设置FPS显示（兼容性）
	 * @param bEnabled 是否显示FPS
	 */
	void SetShowFPSEnabled(bool bEnabled);

	/**
	 * 设置调试信息显示（兼容性）
	 * @param bEnabled 是否显示调试信息
	 */
	void SetDebugInfoEnabled(bool bEnabled);

	/** 获取日志等级索引 */
	UFUNCTION(BlueprintPure, Category = "Advanced")
	int32 GetLogLevelIndex() const { return LogLevelIndex; }

	/**
	 * 设置日志等级
	 * @param Index 日志等级索引 (0=关闭, 1=错误, 2=警告, 3=信息, 4=详细)
	 */
	void SetLogLevelIndex(int32 Index);

	/** 获取性能统计状态 */
	UFUNCTION(BlueprintPure, Category = "Advanced")
	bool GetPerformanceStatsEnabled() const { return bPerformanceStatsEnabled; }

	/**
	 * 设置性能统计显示
	 * @param bEnabled 是否显示性能统计
	 */
	void SetPerformanceStatsEnabled(bool bEnabled);

	/** 获取开发者模式状态 */
	UFUNCTION(BlueprintPure, Category = "Advanced")
	bool GetDeveloperModeEnabled() const { return bDeveloperModeEnabled; }

	/**
	 * 设置开发者模式
	 * @param bEnabled 是否启用开发者模式
	 */
	void SetDeveloperModeEnabled(bool bEnabled);
#pragma endregion 
	// ========================================
	// 工具方法 (Utility Methods)
	// ========================================

	/**
	 * 获取可用分辨率列表
	 * @return 可用分辨率数组
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Settings")
	TArray<FIntPoint> GetAvailableResolutions() const { return AvailableResolutions; }

protected:
	// ========================================
	// 音频设置成员变量
	// ========================================

	/** 主音量 (0.0-1.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float MasterVolume = 1.0f;

	/** 音效音量 (0.0-1.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SFXVolume = 1.0f;

	/** 音乐音量 (0.0-1.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float MusicVolume = 0.8f;

	/** 语音音量 (0.0-1.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float VoiceVolume = 1.0f;

	/** 音频质量索引 (0=低, 1=中, 2=高, 3=极高) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	int32 AudioQualityIndex = 2;

	/** 是否启用静音 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bMuteEnabled = false;

	// ========================================
	// 图形设置成员变量
	// ========================================

	/** 整体图形质量 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 GraphicsQuality = 2;

	/** 分辨率索引 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 ResolutionIndex = 0;

	/** 窗口模式索引 (0=窗口, 1=无边框窗口, 2=全屏) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 WindowModeIndex = 0;

	/** 垂直同步索引 (0=关闭, 1=开启) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 VSyncIndex = 0;

	/** 帧率限制值 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 FrameRateLimit = 60;

	/** 帧率限制索引 (0=无限制, 1=30fps, 2=60fps, 3=90fps, 4=120fps, 5=144fps) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 FrameRateLimitIndex = 2;

	/** 亮度 (0.0-2.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	float Brightness = 1.0f;

	/** 对比度 (0.0-2.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	float Contrast = 1.0f;

	/** 视野距离质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 ViewDistanceIndex = 2;

	/** 抗锯齿质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 AntiAliasingIndex = 2;

	/** 后期处理质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 PostProcessQualityIndex = 2;

	/** 阴影质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 ShadowQualityIndex = 2;

	/** 贴图质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 TextureQualityIndex = 2;

	/** 特效质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 EffectsQualityIndex = 2;

	/** 植被质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 FoliageQualityIndex = 2;

	/** 着色器质量索引 (0=低, 1=中, 2=高, 3=史诗) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 ShadingQualityIndex = 2;

	// ========================================
	// 控制设置成员变量
	// ========================================

	/** 鼠标灵敏度 (0.1-5.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Controls")
	float MouseSensitivity = 1.0f;

	/** 鼠标Y轴反转索引 (0=正常, 1=反转) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Controls")
	int32 InvertMouseYIndex = 0;

	/** 是否启用手柄震动 */
	UPROPERTY(Config)
	bool bControllerVibrationEnabled = true;

	/** 手柄灵敏度 (0.1-5.0) */
	UPROPERTY(Config)
	float ControllerSensitivity = 1.0f;

	// ========================================
	// 游戏性设置成员变量
	// ========================================

	/** 难度等级 (0=简单, 1=普通, 2=困难, 3=地狱) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 DifficultyLevel = 1;

	/** 自动保存索引 (0=关闭, 1=每5分钟, 2=每10分钟, 3=每15分钟) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 AutoSaveIndex = 1;

	/** 字幕索引 (0=关闭, 1=开启) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 SubtitlesIndex = 0;

	/** UI缩放 (0.5-2.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float UIScale = 1.0f;

	// ========================================
	// 辅助功能设置成员变量
	// ========================================

	/** 色盲辅助索引 (0=关闭, 1=红绿色盲, 2=蓝黄色盲, 3=全色盲) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
	int32 ColorBlindFriendlyIndex = 0;

	/** 字体大小 (0.5-2.0) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
	float FontSize = 1.0f;

	/** 按键提示索引 (0=关闭, 1=键盘, 2=手柄, 3=自动) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
	int32 KeyPromptsIndex = 3;

	/** 是否启用高对比度 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
	bool bHighContrastEnabled = false;

	// ========================================
	// 高级设置成员变量
	// ========================================

	/** 调试信息显示索引 (0=关闭, 1=开启) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	int32 ShowDebugInfoIndex = 0;

	/** 日志等级索引 (0=关闭, 1=错误, 2=警告, 3=信息, 4=详细) */
	UPROPERTY(Config)
	int32 LogLevelIndex = 1;

	/** 是否启用性能统计 */
	UPROPERTY(Config)
	bool bPerformanceStatsEnabled = false;

	/** 是否启用开发者模式 */
	UPROPERTY(Config)
	bool bDeveloperModeEnabled = false;

private:
	// 分辨率选项
	TArray<FIntPoint> AvailableResolutions;
	void InitializeResolutions();
	
};
