#include "GameSettingRegistry.h"
#include "GameSettingValueScalar.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingManager.h"
#include "GameSettingValueDynamic.h"
#include "Engine/Engine.h"

UGameSettingRegistry::UGameSettingRegistry()
{
	// 保持现有的初始化逻辑
	RegisteredSettings.Empty();
	SettingValues.Empty();
	SettingCollections.Empty();
}

UGameSettingRegistry* UGameSettingRegistry::Get()
{
	return GetMutableDefault<UGameSettingRegistry>();
}

// ========== 保持现有功能完全不变 ==========
void UGameSettingRegistry::SaveSettings()
{
	SettingValues.Empty();
	
	for (UGameSetting* Setting : RegisteredSettings)
	{
		if (!Setting)
		{
			continue;
		}

		FString DevName = Setting->GetDevName().ToString();
		FString Value;

		// 根据设置类型保存值
		if (UGameSettingValueScalar* ScalarSetting = Cast<UGameSettingValueScalar>(Setting))
		{
			Value = FString::Printf(TEXT("%.6f"), ScalarSetting->GetValue());
		}
		else if (UGameSettingValueDiscrete* DiscreteSetting = Cast<UGameSettingValueDiscrete>(Setting))
		{
			Value = FString::Printf(TEXT("%d"), DiscreteSetting->GetDiscreteOptionIndex());
		}

		if (!Value.IsEmpty())
		{
			SettingValues.Add(DevName, Value);
		}
	}

	// 保存到配置文件
	SaveConfig();
}

void UGameSettingRegistry::LoadSettings()
{
	// 从配置文件加载
	LoadConfig();

	for (UGameSetting* Setting : RegisteredSettings)
	{
		if (!Setting)
		{
			continue;
		}

		FString DevName = Setting->GetDevName().ToString();
		if (FString* SavedValue = SettingValues.Find(DevName))
		{
			// 根据设置类型加载值
			if (UGameSettingValueScalar* ScalarSetting = Cast<UGameSettingValueScalar>(Setting))
			{
				float Value = FCString::Atof(**SavedValue);
				ScalarSetting->SetValue(Value);
			}
			else if (UGameSettingValueDiscrete* DiscreteSetting = Cast<UGameSettingValueDiscrete>(Setting))
			{
				int32 Index = FCString::Atoi(**SavedValue);
				DiscreteSetting->SetDiscreteOptionByIndex(Index);
			}
		}
	}
}

void UGameSettingRegistry::RegisterSetting(UGameSetting* Setting)
{
	if (Setting && !RegisteredSettings.Contains(Setting))
	{
		RegisteredSettings.Add(Setting);
	}
}

void UGameSettingRegistry::UnregisterSetting(UGameSetting* Setting)
{
	if (Setting)
	{
		RegisteredSettings.Remove(Setting);
	}
}

UGameSetting* UGameSettingRegistry::FindSettingByDevName(const FName& DevName) const
{
	for (UGameSetting* Setting : RegisteredSettings)
	{
		if (Setting && Setting->GetDevName() == DevName)
		{
			return Setting;
		}
	}
	
	return nullptr;
}

// ========== 新增：设置初始化功能 ==========
void UGameSettingRegistry::InitializeGameSettings()
{
	if (bIsInitialized)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("开始初始化游戏设置..."));

	// 创建各个设置分类
	UGameSettingCollection* GraphicsSettings = CreateGraphicsSettings();
	UGameSettingCollection* AudioSettings = CreateAudioSettings();
	UGameSettingCollection* ControlsSettings = CreateControlsSettings();
	UGameSettingCollection* GameplaySettings = CreateGameplaySettings();
	UGameSettingCollection* AccessibilitySettings = CreateAccessibilitySettings();
	UGameSettingCollection* AdvancedSettings = CreateAdvancedSettings();

	// 存储到集合映射中
	SettingCollections.Add(TEXT("Graphics"), GraphicsSettings);
	SettingCollections.Add(TEXT("Audio"), AudioSettings);
	SettingCollections.Add(TEXT("Controls"), ControlsSettings);
	SettingCollections.Add(TEXT("Gameplay"), GameplaySettings);
	SettingCollections.Add(TEXT("Accessibility"), AccessibilitySettings);
	SettingCollections.Add(TEXT("Advanced"), AdvancedSettings);
	// 注册所有设置到注册表
	RegisterSetting(GraphicsSettings);
	RegisterSetting(AudioSettings);
	RegisterSetting(ControlsSettings);
	RegisterSetting(GameplaySettings);
	RegisterSetting(AccessibilitySettings);
	RegisterSetting(AdvancedSettings);

	// 加载保存的设置值
	LoadSettings();

	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("游戏设置初始化完成"));
}

UGameSettingCollection* UGameSettingRegistry::GetSettingCollection(const FString& InCategoryName) const
{
	if (TObjectPtr<UGameSettingCollection> const* FoundCollection = SettingCollections.Find(InCategoryName))
	{
		return FoundCollection->Get();
	}
	return nullptr;
}

TArray<UGameSettingCollection*> UGameSettingRegistry::GetAllCollections() const
{
	TArray<UGameSettingCollection*> Collections;
	for (const auto& Pair : SettingCollections)
	{
		if (Pair.Value)
		{
			Collections.Add(Pair.Value.Get());
		}
	}
	return Collections;
}

// ========== 设置创建方法实现 ==========
UGameSettingCollection* UGameSettingRegistry::CreateGraphicsSettings()
{
    UGameSettingCollection* GraphicsCollection = NewObject<UGameSettingCollection>(this);
    GraphicsCollection->SetDevName(TEXT("Graphics"));
    GraphicsCollection->SetDisplayName(FText::FromString(TEXT("图形设置")));
    GraphicsCollection->SetDescriptionRichText(FText::FromString(TEXT("调整游戏的图形质量和视觉效果设置")));

    // 通用质量选项
    TArray<FText> QualityOptions;
    QualityOptions.Add(FText::FromString(TEXT("低")));
    QualityOptions.Add(FText::FromString(TEXT("中")));
    QualityOptions.Add(FText::FromString(TEXT("高")));
    QualityOptions.Add(FText::FromString(TEXT("史诗")));

    // 1. 整体图形质量设置
    UGameSettingValueDiscreteDynamic* OverallQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    OverallQualitySetting->SetDevName(TEXT("OverallGraphicsQuality"));
    OverallQualitySetting->SetDisplayName(FText::FromString(TEXT("整体画质")));
    OverallQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制游戏的整体图形质量等级。较高的设置会提供更好的视觉效果，但可能影响性能。")));
    OverallQualitySetting->SetOptions(QualityOptions);
    OverallQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetGraphicsQuality();
    }));
    OverallQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetGraphicsQuality(Index);
    }));
    GraphicsCollection->AddSetting(OverallQualitySetting);

    // 2. 视野距离设置
    UGameSettingValueDiscreteDynamic* ViewDistanceSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    ViewDistanceSetting->SetDevName(TEXT("ViewDistance"));
    ViewDistanceSetting->SetDisplayName(FText::FromString(TEXT("视野距离")));
    ViewDistanceSetting->SetDescriptionRichText(FText::FromString(TEXT("控制游戏中可见物体的最远距离。较高的设置可以看到更远的物体，但会增加GPU负担。")));
    ViewDistanceSetting->SetOptions(QualityOptions);
    ViewDistanceSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetViewDistanceIndex();
    }));
    ViewDistanceSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetViewDistanceIndex(Index);
    }));
    GraphicsCollection->AddSetting(ViewDistanceSetting);

    // 3. 抗锯齿设置
    TArray<FText> AntiAliasingOptions;
    AntiAliasingOptions.Add(FText::FromString(TEXT("关闭")));
    AntiAliasingOptions.Add(FText::FromString(TEXT("FXAA")));
    AntiAliasingOptions.Add(FText::FromString(TEXT("TAA")));
    AntiAliasingOptions.Add(FText::FromString(TEXT("MSAA")));

    UGameSettingValueDiscreteDynamic* AntiAliasingSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    AntiAliasingSetting->SetDevName(TEXT("AntiAliasing"));
    AntiAliasingSetting->SetDisplayName(FText::FromString(TEXT("抗锯齿")));
    AntiAliasingSetting->SetDescriptionRichText(FText::FromString(TEXT("减少图像边缘的锯齿效果，使画面更加平滑。FXAA性能最好，TAA平衡性能和质量，MSAA质量最高但性能消耗大。")));
    AntiAliasingSetting->SetOptions(AntiAliasingOptions);
    AntiAliasingSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetAntiAliasingIndex();
    }));
    AntiAliasingSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetAntiAliasingIndex(Index);
    }));
    GraphicsCollection->AddSetting(AntiAliasingSetting);

    // 4. 后期处理质量
    UGameSettingValueDiscreteDynamic* PostProcessSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    PostProcessSetting->SetDevName(TEXT("PostProcessQuality"));
    PostProcessSetting->SetDisplayName(FText::FromString(TEXT("后期处理质量")));
    PostProcessSetting->SetDescriptionRichText(FText::FromString(TEXT("控制屏幕空间反射、环境光遮蔽、景深等后期处理效果的质量。")));
    PostProcessSetting->SetOptions(QualityOptions);
    PostProcessSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetPostProcessQualityIndex();
    }));
    PostProcessSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetPostProcessQualityIndex(Index);
    }));
    GraphicsCollection->AddSetting(PostProcessSetting);

    // 5. 阴影质量
    UGameSettingValueDiscreteDynamic* ShadowQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    ShadowQualitySetting->SetDevName(TEXT("ShadowQuality"));
    ShadowQualitySetting->SetDisplayName(FText::FromString(TEXT("阴影质量")));
    ShadowQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制阴影的分辨率和细节程度。较高的设置会产生更清晰、更真实的阴影效果。")));
    ShadowQualitySetting->SetOptions(QualityOptions);
    ShadowQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetShadowQualityIndex();
    }));
    ShadowQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetShadowQualityIndex(Index);
    }));
    GraphicsCollection->AddSetting(ShadowQualitySetting);

    // 6. 贴图质量
    UGameSettingValueDiscreteDynamic* TextureQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    TextureQualitySetting->SetDevName(TEXT("TextureQuality"));
    TextureQualitySetting->SetDisplayName(FText::FromString(TEXT("贴图质量")));
    TextureQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制游戏中纹理贴图的分辨率和细节。较高的设置会显示更清晰的纹理，但需要更多显存。")));
    TextureQualitySetting->SetOptions(QualityOptions);
    TextureQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetTextureQualityIndex();
    }));
    TextureQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetTextureQualityIndex(Index);
    }));
    GraphicsCollection->AddSetting(TextureQualitySetting);

    // 7. 特效质量
    UGameSettingValueDiscreteDynamic* EffectsQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    EffectsQualitySetting->SetDevName(TEXT("EffectsQuality"));
    EffectsQualitySetting->SetDisplayName(FText::FromString(TEXT("特效质量")));
    EffectsQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制粒子效果、爆炸、魔法等视觉特效的质量和复杂度。")));
    EffectsQualitySetting->SetOptions(QualityOptions);
    EffectsQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetEffectsQualityIndex();
    }));
    EffectsQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetEffectsQualityIndex(Index);
    }));
    GraphicsCollection->AddSetting(EffectsQualitySetting);

    // 8. 植被质量
    UGameSettingValueDiscreteDynamic* FoliageQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    FoliageQualitySetting->SetDevName(TEXT("FoliageQuality"));
    FoliageQualitySetting->SetDisplayName(FText::FromString(TEXT("植被质量")));
    FoliageQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制草地、树木等植被的密度和细节程度。较高的设置会显示更多更详细的植被。")));
    FoliageQualitySetting->SetOptions(QualityOptions);
    FoliageQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetFoliageQualityIndex();
    }));
    FoliageQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetFoliageQualityIndex(Index);
    }));
    GraphicsCollection->AddSetting(FoliageQualitySetting);

    // 9. 着色器质量
    UGameSettingValueDiscreteDynamic* ShadingQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    ShadingQualitySetting->SetDevName(TEXT("ShadingQuality"));
    ShadingQualitySetting->SetDisplayName(FText::FromString(TEXT("着色器质量")));
    ShadingQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制材质着色和光照计算的复杂度。影响物体表面的光泽、反射等视觉效果。")));
    ShadingQualitySetting->SetOptions(QualityOptions);
    ShadingQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]()
    {
        return UGameSettingManager::Get()->GetShadingQualityIndex();
    }));
    ShadingQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index)
    {
        UGameSettingManager::Get()->SetShadingQualityIndex(Index);
    }));
    GraphicsCollection->AddSetting(ShadingQualitySetting);

    // 10. 分辨率设置
    UGameSettingValueDiscreteDynamic* ResolutionSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    ResolutionSetting->SetDevName(TEXT("Resolution"));
    ResolutionSetting->SetDisplayName(FText::FromString(TEXT("分辨率")));
    ResolutionSetting->SetDescriptionRichText(FText::FromString(TEXT("设置游戏的显示分辨率。较高的分辨率提供更清晰的画面，但需要更强的显卡性能。")));
    
    TArray<FText> ResolutionOptions;
	ResolutionOptions.Add(FText::FromString(TEXT("1366x768")));
	ResolutionOptions.Add(FText::FromString(TEXT("1600x900")));
    ResolutionOptions.Add(FText::FromString(TEXT("1920x1080")));
    ResolutionOptions.Add(FText::FromString(TEXT("2560x1440")));
    ResolutionOptions.Add(FText::FromString(TEXT("3840x2160")));
    ResolutionSetting->SetOptions(ResolutionOptions);
    
    ResolutionSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
        return UGameSettingManager::Get()->GetResolutionIndex();
    }));
    ResolutionSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
        UGameSettingManager::Get()->SetResolutionIndex(Index);
    }));
    GraphicsCollection->AddSetting(ResolutionSetting);

    // 11. 窗口模式
    UGameSettingValueDiscreteDynamic* WindowModeSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    WindowModeSetting->SetDevName(TEXT("WindowMode"));
    WindowModeSetting->SetDisplayName(FText::FromString(TEXT("窗口模式")));
    WindowModeSetting->SetDescriptionRichText(FText::FromString(TEXT("选择游戏的显示模式。全屏模式性能最佳，窗口化便于多任务，无边框窗口兼顾两者优点。")));
    
    TArray<FText> WindowModeOptions;
    WindowModeOptions.Add(FText::FromString(TEXT("全屏")));
    WindowModeOptions.Add(FText::FromString(TEXT("窗口化")));
    WindowModeOptions.Add(FText::FromString(TEXT("无边框窗口")));
    WindowModeSetting->SetOptions(WindowModeOptions);
    
    WindowModeSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
        return UGameSettingManager::Get()->GetWindowModeIndex();
    }));
    WindowModeSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
        UGameSettingManager::Get()->SetWindowModeIndex(Index);
    }));
    GraphicsCollection->AddSetting(WindowModeSetting);

    // 12. 垂直同步
    UGameSettingValueDiscreteDynamic* VSyncSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    VSyncSetting->SetDevName(TEXT("VSync"));
    VSyncSetting->SetDisplayName(FText::FromString(TEXT("垂直同步")));
    VSyncSetting->SetDescriptionRichText(FText::FromString(TEXT("防止画面撕裂现象，但可能增加输入延迟。建议在高刷新率显示器上关闭。")));
    
    TArray<FText> VSyncOptions;
    VSyncOptions.Add(FText::FromString(TEXT("关闭")));
    VSyncOptions.Add(FText::FromString(TEXT("开启")));
    VSyncSetting->SetOptions(VSyncOptions);
    
    VSyncSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
        return UGameSettingManager::Get()->GetVSyncEnabled() ? 1 : 0;
    }));
    VSyncSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
        UGameSettingManager::Get()->SetVSyncEnabled(Index == 1);
    }));
    GraphicsCollection->AddSetting(VSyncSetting);

    // 13. 帧率限制
    UGameSettingValueDiscreteDynamic* FrameRateLimitSetting = NewObject<UGameSettingValueDiscreteDynamic>(GraphicsCollection);
    FrameRateLimitSetting->SetDevName(TEXT("FrameRateLimit"));
    FrameRateLimitSetting->SetDisplayName(FText::FromString(TEXT("帧率限制")));
    FrameRateLimitSetting->SetDescriptionRichText(FText::FromString(TEXT("限制游戏的最大帧率，可以减少GPU负载和发热。建议设置为显示器刷新率的倍数。")));

    TArray<FText> FrameRateOptions;
    FrameRateOptions.Add(FText::FromString(TEXT("无限制")));
    FrameRateOptions.Add(FText::FromString(TEXT("30 FPS")));
    FrameRateOptions.Add(FText::FromString(TEXT("60 FPS")));
    FrameRateOptions.Add(FText::FromString(TEXT("120 FPS")));
    FrameRateOptions.Add(FText::FromString(TEXT("144 FPS")));
    FrameRateLimitSetting->SetOptions(FrameRateOptions);

    FrameRateLimitSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
        return UGameSettingManager::Get()->GetFrameRateLimitIndex();
    }));
    FrameRateLimitSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
        UGameSettingManager::Get()->SetFrameRateLimitIndex(Index);
    }));
    GraphicsCollection->AddSetting(FrameRateLimitSetting);

    return GraphicsCollection;
}

UGameSettingCollection* UGameSettingRegistry::CreateAudioSettings()
{
	UGameSettingCollection* AudioCollection = NewObject<UGameSettingCollection>(this);
	AudioCollection->SetDevName(TEXT("Audio"));
	AudioCollection->SetDisplayName(NSLOCTEXT("GameSettings", "Audio", "音频设置"));
	AudioCollection->SetDescriptionRichText(FText::FromString(TEXT("调整游戏的音频音量和音质设置")));

	// 主音量设置
	UGameSettingValueDynamic* MasterVolumeSetting = NewObject<UGameSettingValueDynamic>(AudioCollection);
	MasterVolumeSetting->SetDevName(TEXT("MasterVolume"));
	MasterVolumeSetting->SetDisplayName(NSLOCTEXT("GameSettings", "MasterVolume", "主音量"));
	MasterVolumeSetting->SetDescriptionRichText(FText::FromString(TEXT("控制游戏的整体音量大小，影响所有音频输出。")));
	MasterVolumeSetting->SetMinimumValue(0.0f);
	MasterVolumeSetting->SetMaximumValue(1.0f);
	MasterVolumeSetting->SetStepSize(0.1f);
	
	MasterVolumeSetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]()
	{
		return UGameSettingManager::Get()->GetMasterVolume();
	}));
	
	MasterVolumeSetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Volume)
	{
		UGameSettingManager::Get()->SetMasterVolume(Volume);
	}));
	
	AudioCollection->AddSetting(MasterVolumeSetting);

	// 音效音量设置
	UGameSettingValueDynamic* SFXVolumeSetting = NewObject<UGameSettingValueDynamic>(AudioCollection);
	SFXVolumeSetting->SetDevName(TEXT("SFXVolume"));
	SFXVolumeSetting->SetDisplayName(FText::FromString(TEXT("音效音量")));
	SFXVolumeSetting->SetDescriptionRichText(FText::FromString(TEXT("控制游戏中音效的音量，包括脚步声、武器声音、环境音效等。")));
	SFXVolumeSetting->SetMinimumValue(0.0f);
	SFXVolumeSetting->SetMaximumValue(1.0f);
	SFXVolumeSetting->SetStepSize(0.1f);
	
	SFXVolumeSetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetSFXVolume();
	}));
	SFXVolumeSetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Volume) {
		UGameSettingManager::Get()->SetSFXVolume(Volume);
	}));
	AudioCollection->AddSetting(SFXVolumeSetting);

	// 音乐音量设置
	UGameSettingValueDynamic* MusicVolumeSetting = NewObject<UGameSettingValueDynamic>(AudioCollection);
	MusicVolumeSetting->SetDevName(TEXT("MusicVolume"));
	MusicVolumeSetting->SetDisplayName(FText::FromString(TEXT("音乐音量")));
	MusicVolumeSetting->SetDescriptionRichText(FText::FromString(TEXT("控制游戏背景音乐的音量大小。")));
	MusicVolumeSetting->SetMinimumValue(0.0f);
	MusicVolumeSetting->SetMaximumValue(1.0f);
	MusicVolumeSetting->SetStepSize(0.1f);
	
	MusicVolumeSetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetMusicVolume();
	}));
	MusicVolumeSetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Volume) {
		UGameSettingManager::Get()->SetMusicVolume(Volume);
	}));
	AudioCollection->AddSetting(MusicVolumeSetting);

	// 语音音量设置
	UGameSettingValueDynamic* VoiceVolumeSetting = NewObject<UGameSettingValueDynamic>(AudioCollection);
	VoiceVolumeSetting->SetDevName(TEXT("VoiceVolume"));
	VoiceVolumeSetting->SetDisplayName(FText::FromString(TEXT("语音音量")));
	VoiceVolumeSetting->SetDescriptionRichText(FText::FromString(TEXT("控制角色对话和语音提示的音量大小。")));
	VoiceVolumeSetting->SetMinimumValue(0.0f);
	VoiceVolumeSetting->SetMaximumValue(1.0f);
	VoiceVolumeSetting->SetStepSize(0.1f);
	
	VoiceVolumeSetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetVoiceVolume();
	}));
	VoiceVolumeSetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Volume) {
		UGameSettingManager::Get()->SetVoiceVolume(Volume);
	}));
	AudioCollection->AddSetting(VoiceVolumeSetting);

	// 音频质量设置
	UGameSettingValueDiscreteDynamic* AudioQualitySetting = NewObject<UGameSettingValueDiscreteDynamic>(AudioCollection);
	AudioQualitySetting->SetDevName(TEXT("AudioQuality"));
	AudioQualitySetting->SetDisplayName(FText::FromString(TEXT("音频质量")));
	AudioQualitySetting->SetDescriptionRichText(FText::FromString(TEXT("控制音频的采样率和压缩质量。较高的设置提供更好的音质，但占用更多内存。")));
	
	TArray<FText> AudioQualityOptions;
	AudioQualityOptions.Add(FText::FromString(TEXT("低")));
	AudioQualityOptions.Add(FText::FromString(TEXT("中")));
	AudioQualityOptions.Add(FText::FromString(TEXT("高")));
	AudioQualityOptions.Add(FText::FromString(TEXT("极高")));
	AudioQualitySetting->SetOptions(AudioQualityOptions);
	
	AudioQualitySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetAudioQualityIndex();
	}));
	AudioQualitySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetAudioQualityIndex(Index);
	}));
	AudioCollection->AddSetting(AudioQualitySetting);

	// 静音设置
	UGameSettingValueDiscreteDynamic* MuteSetting = NewObject<UGameSettingValueDiscreteDynamic>(AudioCollection);
	MuteSetting->SetDevName(TEXT("Mute"));
	MuteSetting->SetDisplayName(FText::FromString(TEXT("静音")));
	
	TArray<FText> MuteOptions;
	MuteOptions.Add(FText::FromString(TEXT("关闭")));
	MuteOptions.Add(FText::FromString(TEXT("开启")));
	MuteSetting->SetOptions(MuteOptions);
	
	MuteSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetMuteEnabled() ? 1 : 0;
	}));
	MuteSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetMuteEnabled(Index == 1);
	}));
	AudioCollection->AddSetting(MuteSetting);

	return AudioCollection;
}

UGameSettingCollection* UGameSettingRegistry::CreateControlsSettings()
{
	UGameSettingCollection* ControlsCollection = NewObject<UGameSettingCollection>(this);
	ControlsCollection->SetDevName(TEXT("Controls"));
	ControlsCollection->SetDisplayName(NSLOCTEXT("GameSettings", "Controls", "控制设置"));
	ControlsCollection->SetDescriptionRichText(FText::FromString(TEXT("调整鼠标、键盘和手柄的控制设置，以获得最佳的游戏操作体验。")));

	// 鼠标灵敏度设置
	UGameSettingValueDynamic* MouseSensitivitySetting = NewObject<UGameSettingValueDynamic>(ControlsCollection);
	MouseSensitivitySetting->SetDevName(TEXT("MouseSensitivity"));
	MouseSensitivitySetting->SetDisplayName(FText::FromString(TEXT("鼠标灵敏度")));
	MouseSensitivitySetting->SetDescriptionRichText(FText::FromString(TEXT("调整鼠标移动的灵敏度。数值越高，鼠标移动越快。建议根据个人习惯调整到舒适的水平。")));
	MouseSensitivitySetting->SetMinimumValue(0.1f);
	MouseSensitivitySetting->SetMaximumValue(5.0f);
	MouseSensitivitySetting->SetStepSize(0.1f);
	
	MouseSensitivitySetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetMouseSensitivity();
	}));
	MouseSensitivitySetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Value) {
		UGameSettingManager::Get()->SetMouseSensitivity(Value);
	}));
	ControlsCollection->AddSetting(MouseSensitivitySetting);

	// 鼠标反转Y轴
	UGameSettingValueDiscreteDynamic* InvertMouseYSetting = NewObject<UGameSettingValueDiscreteDynamic>(ControlsCollection);
	InvertMouseYSetting->SetDevName(TEXT("InvertMouseY"));
	InvertMouseYSetting->SetDisplayName(FText::FromString(TEXT("反转鼠标Y轴")));
	InvertMouseYSetting->SetDescriptionRichText(FText::FromString(TEXT("开启后，向上移动鼠标将使视角向下，向下移动鼠标将使视角向上。适合习惯飞行模拟器操作的玩家。")));
	
	TArray<FText> InvertOptions;
	InvertOptions.Add(FText::FromString(TEXT("关闭")));
	InvertOptions.Add(FText::FromString(TEXT("开启")));
	InvertMouseYSetting->SetOptions(InvertOptions);
	
	InvertMouseYSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetInvertMouseY() ? 1 : 0;
	}));
	InvertMouseYSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetInvertMouseY(Index == 1);
	}));
	ControlsCollection->AddSetting(InvertMouseYSetting);

	// 手柄振动
	UGameSettingValueDiscreteDynamic* ControllerVibrationSetting = NewObject<UGameSettingValueDiscreteDynamic>(ControlsCollection);
	ControllerVibrationSetting->SetDevName(TEXT("ControllerVibration"));
	ControllerVibrationSetting->SetDisplayName(FText::FromString(TEXT("手柄振动")));
	ControllerVibrationSetting->SetDescriptionRichText(FText::FromString(TEXT("开启后，游戏中的特定事件（如受到伤害、开火等）会触发手柄振动反馈，增强游戏沉浸感。")));
	ControllerVibrationSetting->SetOptions(InvertOptions);
	
	ControllerVibrationSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetControllerVibrationEnabled() ? 1 : 0;
	}));
	ControllerVibrationSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetControllerVibrationEnabled(Index == 1);
	}));
	ControlsCollection->AddSetting(ControllerVibrationSetting);

	// 手柄灵敏度
	UGameSettingValueDynamic* ControllerSensitivitySetting = NewObject<UGameSettingValueDynamic>(ControlsCollection);
	ControllerSensitivitySetting->SetDevName(TEXT("ControllerSensitivity"));
	ControllerSensitivitySetting->SetDisplayName(FText::FromString(TEXT("手柄灵敏度")));
	ControllerSensitivitySetting->SetDescriptionRichText(FText::FromString(TEXT("调整手柄摇杆的灵敏度。数值越高，摇杆移动响应越快。建议根据个人习惯调整。")));
	ControllerSensitivitySetting->SetMinimumValue(0.1f);
	ControllerSensitivitySetting->SetMaximumValue(3.0f);
	ControllerSensitivitySetting->SetStepSize(0.1f);
	
	ControllerSensitivitySetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetControllerSensitivity();
	}));
	ControllerSensitivitySetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Value) {
		UGameSettingManager::Get()->SetControllerSensitivity(Value);
	}));
	ControlsCollection->AddSetting(ControllerSensitivitySetting);

	return ControlsCollection;
}

UGameSettingCollection* UGameSettingRegistry::CreateGameplaySettings()
{
	UGameSettingCollection* GameplayCollection = NewObject<UGameSettingCollection>(this);
	GameplayCollection->SetDevName(TEXT("Gameplay"));
	GameplayCollection->SetDisplayName(NSLOCTEXT("GameSettings", "Gameplay", "游戏性设置"));
	GameplayCollection->SetDescriptionRichText(FText::FromString(TEXT("调整游戏难度、界面显示和其他游戏体验相关的设置。")));

	// 难度设置
	UGameSettingValueDiscreteDynamic* DifficultySetting = NewObject<UGameSettingValueDiscreteDynamic>(GameplayCollection);
	DifficultySetting->SetDevName(TEXT("Difficulty"));
	DifficultySetting->SetDisplayName(FText::FromString(TEXT("游戏难度")));
	DifficultySetting->SetDescriptionRichText(FText::FromString(TEXT("选择游戏难度等级。简单模式适合新手，困难模式提供更大挑战。")));
	
	TArray<FText> DifficultyOptions;
	DifficultyOptions.Add(FText::FromString(TEXT("简单")));
	DifficultyOptions.Add(FText::FromString(TEXT("普通")));
	DifficultyOptions.Add(FText::FromString(TEXT("困难")));
	DifficultySetting->SetOptions(DifficultyOptions);
	
	DifficultySetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetDifficultyLevel();
	}));
	DifficultySetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetDifficultyLevel(Index);
	}));
	GameplayCollection->AddSetting(DifficultySetting);

	// 自动保存
	UGameSettingValueDiscreteDynamic* AutoSaveSetting = NewObject<UGameSettingValueDiscreteDynamic>(GameplayCollection);
	AutoSaveSetting->SetDevName(TEXT("AutoSave"));
	AutoSaveSetting->SetDisplayName(FText::FromString(TEXT("自动保存")));
	AutoSaveSetting->SetDescriptionRichText(FText::FromString(TEXT("开启后，游戏会在特定时间点自动保存进度，避免意外丢失游戏数据。")));
	
	TArray<FText> AutoSaveOptions;
	AutoSaveOptions.Add(FText::FromString(TEXT("关闭")));
	AutoSaveOptions.Add(FText::FromString(TEXT("开启")));
	AutoSaveSetting->SetOptions(AutoSaveOptions);
	
	AutoSaveSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetAutoSaveIndex() ? 1 : 0;
	}));
	AutoSaveSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetAutoSaveIndex(Index == 1);
	}));
	GameplayCollection->AddSetting(AutoSaveSetting);

	// UI缩放
	UGameSettingValueDynamic* UIScaleSetting = NewObject<UGameSettingValueDynamic>(GameplayCollection);
	UIScaleSetting->SetDevName(TEXT("UIScale"));
	UIScaleSetting->SetDisplayName(FText::FromString(TEXT("UI缩放")));
	UIScaleSetting->SetDescriptionRichText(FText::FromString(TEXT("调整用户界面的大小。数值越大，UI元素显示越大，适合高分辨率显示器或视力不佳的玩家。")));
	UIScaleSetting->SetMinimumValue(0.5f);
	UIScaleSetting->SetMaximumValue(2.0f);
	UIScaleSetting->SetStepSize(0.1f);
	
	UIScaleSetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetUIScale();
	}));
	UIScaleSetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Value) {
		UGameSettingManager::Get()->SetUIScale(Value);
	}));
	GameplayCollection->AddSetting(UIScaleSetting);

	// 字幕显示
	UGameSettingValueDiscreteDynamic* SubtitlesSetting = NewObject<UGameSettingValueDiscreteDynamic>(GameplayCollection);
	SubtitlesSetting->SetDevName(TEXT("Subtitles"));
	SubtitlesSetting->SetDisplayName(FText::FromString(TEXT("字幕显示")));
	SubtitlesSetting->SetDescriptionRichText(FText::FromString(TEXT("开启后，游戏中的对话和音效会显示字幕文本，有助于理解游戏内容或在无声环境下游戏。")));
	
	TArray<FText> SubtitleOptions;
	SubtitleOptions.Add(FText::FromString(TEXT("关闭")));
	SubtitleOptions.Add(FText::FromString(TEXT("开启")));
	SubtitlesSetting->SetOptions(SubtitleOptions);
	
	SubtitlesSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetSubtitlesEnabled() ? 1 : 0;
	}));
	SubtitlesSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetSubtitlesEnabled(Index == 1);
	}));
	GameplayCollection->AddSetting(SubtitlesSetting);

	return GameplayCollection;
}

UGameSettingCollection* UGameSettingRegistry::CreateAccessibilitySettings()
{
	UGameSettingCollection* AccessibilityCollection = NewObject<UGameSettingCollection>(this);
	AccessibilityCollection->SetDevName(TEXT("Accessibility"));
	AccessibilityCollection->SetDisplayName(NSLOCTEXT("GameSettings", "Accessibility", "辅助功能设置"));
	AccessibilityCollection->SetDescriptionRichText(FText::FromString(TEXT("为有特殊需求的玩家提供辅助功能，提升游戏的可访问性和包容性。")));

	// 色盲辅助
	UGameSettingValueDiscreteDynamic* ColorBlindSetting = NewObject<UGameSettingValueDiscreteDynamic>(AccessibilityCollection);
	ColorBlindSetting->SetDevName(TEXT("ColorBlindAssist"));
	ColorBlindSetting->SetDisplayName(FText::FromString(TEXT("色盲辅助")));
	ColorBlindSetting->SetDescriptionRichText(FText::FromString(TEXT("为色盲或色弱玩家提供颜色辅助功能，通过调整色彩方案或添加图案来区分不同元素。")));
	
	TArray<FText> ColorBlindOptions;
	ColorBlindOptions.Add(FText::FromString(TEXT("关闭")));
	ColorBlindOptions.Add(FText::FromString(TEXT("红绿色盲")));
	ColorBlindOptions.Add(FText::FromString(TEXT("蓝黄色盲")));
	ColorBlindSetting->SetOptions(ColorBlindOptions);
	
	ColorBlindSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetColorBlindAssistIndex();
	}));
	ColorBlindSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetColorBlindFriendlyIndex(Index);
	}));
	AccessibilityCollection->AddSetting(ColorBlindSetting);

	// 字幕大小
	UGameSettingValueDynamic* SubtitleSizeSetting = NewObject<UGameSettingValueDynamic>(AccessibilityCollection);
	SubtitleSizeSetting->SetDevName(TEXT("SubtitleSize"));
	SubtitleSizeSetting->SetDisplayName(FText::FromString(TEXT("字幕大小")));
	SubtitleSizeSetting->SetDescriptionRichText(FText::FromString(TEXT("调整字幕文本的大小。较大的字幕更容易阅读，适合视力不佳或使用大屏幕的玩家。")));
	SubtitleSizeSetting->SetMinimumValue(0.5f);
	SubtitleSizeSetting->SetMaximumValue(2.0f);
	SubtitleSizeSetting->SetStepSize(0.1f);
	
	SubtitleSizeSetting->SetDynamicGetter(FGameSettingDynamicGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetSubtitleSize();
	}));
	SubtitleSizeSetting->SetDynamicSetter(FGameSettingDynamicSetter::CreateLambda([](float Value) {
		UGameSettingManager::Get()->SetSubtitleSize(Value);
	}));
	AccessibilityCollection->AddSetting(SubtitleSizeSetting);

	// 按键提示
	UGameSettingValueDiscreteDynamic* InputPromptsSetting = NewObject<UGameSettingValueDiscreteDynamic>(AccessibilityCollection);
	InputPromptsSetting->SetDevName(TEXT("InputPrompts"));
	InputPromptsSetting->SetDisplayName(FText::FromString(TEXT("按键提示")));
	InputPromptsSetting->SetDescriptionRichText(FText::FromString(TEXT("显示操作按键的提示信息，帮助新手玩家快速熟悉游戏操作，或为记忆力不佳的玩家提供帮助。")));
	
	TArray<FText> InputPromptOptions;
	InputPromptOptions.Add(FText::FromString(TEXT("关闭")));
	InputPromptOptions.Add(FText::FromString(TEXT("开启")));
	InputPromptsSetting->SetOptions(InputPromptOptions);
	
	InputPromptsSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetKeyPromptsIndex() ? 1 : 0;
	}));
	InputPromptsSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetKeyPromptsIndex(Index == 1);
	}));
	AccessibilityCollection->AddSetting(InputPromptsSetting);

	// 高对比度模式
	UGameSettingValueDiscreteDynamic* HighContrastSetting = NewObject<UGameSettingValueDiscreteDynamic>(AccessibilityCollection);
	HighContrastSetting->SetDevName(TEXT("HighContrast"));
	HighContrastSetting->SetDisplayName(FText::FromString(TEXT("高对比度模式")));
	HighContrastSetting->SetDescriptionRichText(FText::FromString(TEXT("增强界面元素的对比度，使文本和图标更加清晰可见，适合视力不佳或在强光环境下游戏的玩家。")));
	
	TArray<FText> HighContrastOptions;
	HighContrastOptions.Add(FText::FromString(TEXT("关闭")));
	HighContrastOptions.Add(FText::FromString(TEXT("开启")));
	HighContrastSetting->SetOptions(HighContrastOptions);
	
	HighContrastSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetHighContrastEnabled() ? 1 : 0;
	}));
	HighContrastSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetHighContrastEnabled(Index == 1);
	}));
	AccessibilityCollection->AddSetting(HighContrastSetting);

	return AccessibilityCollection;
}

UGameSettingCollection* UGameSettingRegistry::CreateAdvancedSettings()
{
	UGameSettingCollection* AdvancedCollection = NewObject<UGameSettingCollection>(this);
	AdvancedCollection->SetDevName(TEXT("Advanced"));
	AdvancedCollection->SetDisplayName(NSLOCTEXT("GameSettings", "Advanced", "高级设置"));
	AdvancedCollection->SetDescriptionRichText(FText::FromString(TEXT("面向高级用户和开发者的设置选项，包括调试信息、性能监控和开发者工具。")));

	// 显示FPS
	UGameSettingValueDiscreteDynamic* ShowFPSSetting = NewObject<UGameSettingValueDiscreteDynamic>(AdvancedCollection);
	ShowFPSSetting->SetDevName(TEXT("ShowFPS"));
	ShowFPSSetting->SetDisplayName(FText::FromString(TEXT("显示FPS")));
	ShowFPSSetting->SetDescriptionRichText(FText::FromString(TEXT("在屏幕上显示实时帧率信息，帮助监控游戏性能和优化设置。")));
	
	TArray<FText> ShowFPSOptions;
	ShowFPSOptions.Add(FText::FromString(TEXT("关闭")));
	ShowFPSOptions.Add(FText::FromString(TEXT("开启")));
	ShowFPSSetting->SetOptions(ShowFPSOptions);
	
	ShowFPSSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetShowFPSEnabled() ? 1 : 0;
	}));
	ShowFPSSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetShowFPSEnabled(Index == 1);
	}));
	AdvancedCollection->AddSetting(ShowFPSSetting);

	// 调试信息
	UGameSettingValueDiscreteDynamic* DebugInfoSetting = NewObject<UGameSettingValueDiscreteDynamic>(AdvancedCollection);
	DebugInfoSetting->SetDevName(TEXT("DebugInfo"));
	DebugInfoSetting->SetDisplayName(FText::FromString(TEXT("调试信息")));
	DebugInfoSetting->SetDescriptionRichText(FText::FromString(TEXT("显示详细的调试信息，包括内存使用、渲染统计等。主要用于开发和问题诊断。")));
	DebugInfoSetting->SetOptions(ShowFPSOptions);
	
	DebugInfoSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetDebugInfoEnabled() ? 1 : 0;
	}));
	DebugInfoSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetDebugInfoEnabled(Index == 1);
	}));
	AdvancedCollection->AddSetting(DebugInfoSetting);

	// 日志级别
	UGameSettingValueDiscreteDynamic* LogLevelSetting = NewObject<UGameSettingValueDiscreteDynamic>(AdvancedCollection);
	LogLevelSetting->SetDevName(TEXT("LogLevel"));
	LogLevelSetting->SetDisplayName(FText::FromString(TEXT("日志级别")));
	LogLevelSetting->SetDescriptionRichText(FText::FromString(TEXT("设置游戏日志的详细程度。错误级别只记录严重问题，详细级别记录所有信息，用于问题排查。")));
	
	TArray<FText> LogLevelOptions;
	LogLevelOptions.Add(FText::FromString(TEXT("错误")));
	LogLevelOptions.Add(FText::FromString(TEXT("警告")));
	LogLevelOptions.Add(FText::FromString(TEXT("信息")));
	LogLevelOptions.Add(FText::FromString(TEXT("详细")));
	LogLevelSetting->SetOptions(LogLevelOptions);
	
	LogLevelSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetLogLevelIndex();
	}));
	LogLevelSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetLogLevelIndex(Index);
	}));
	AdvancedCollection->AddSetting(LogLevelSetting);

	// 性能统计
	UGameSettingValueDiscreteDynamic* PerformanceStatsSetting = NewObject<UGameSettingValueDiscreteDynamic>(AdvancedCollection);
	PerformanceStatsSetting->SetDevName(TEXT("PerformanceStats"));
	PerformanceStatsSetting->SetDisplayName(FText::FromString(TEXT("性能统计")));
	PerformanceStatsSetting->SetDescriptionRichText(FText::FromString(TEXT("显示详细的性能统计信息，包括CPU、GPU使用率和内存占用，用于性能分析和优化。")));
	PerformanceStatsSetting->SetOptions(ShowFPSOptions);
	
	PerformanceStatsSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetPerformanceStatsEnabled() ? 1 : 0;
	}));
	PerformanceStatsSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetPerformanceStatsEnabled(Index == 1);
	}));
	AdvancedCollection->AddSetting(PerformanceStatsSetting);

	// 开发者模式
	UGameSettingValueDiscreteDynamic* DeveloperModeSetting = NewObject<UGameSettingValueDiscreteDynamic>(AdvancedCollection);
	DeveloperModeSetting->SetDevName(TEXT("DeveloperMode"));
	DeveloperModeSetting->SetDisplayName(FText::FromString(TEXT("开发者模式")));
	DeveloperModeSetting->SetDescriptionRichText(FText::FromString(TEXT("启用开发者专用功能，包括控制台命令、调试工具和实验性功能。仅建议开发人员使用。")));
	DeveloperModeSetting->SetOptions(ShowFPSOptions);
	
	DeveloperModeSetting->SetDynamicGetter(FGameSettingDiscreteGetter::CreateLambda([]() {
		return UGameSettingManager::Get()->GetDeveloperModeEnabled() ? 1 : 0;
	}));
	DeveloperModeSetting->SetDynamicSetter(FGameSettingDiscreteSetter::CreateLambda([](int32 Index) {
		UGameSettingManager::Get()->SetDeveloperModeEnabled(Index == 1);
	}));
	AdvancedCollection->AddSetting(DeveloperModeSetting);

	return AdvancedCollection;
}