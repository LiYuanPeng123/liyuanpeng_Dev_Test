#include "GameSetting.h"
#include "Engine/Engine.h"
//////////////////////////////////////////////////////////////////////////
// FGameSettingFilterState

/**
 * 构造函数
 * 初始化过滤器状态的默认值
 */
FGameSettingFilterState::FGameSettingFilterState()
: SearchTextEvaluator(ETextFilterExpressionEvaluatorMode::BasicString)

{
	bIncludeDisabled = true;
	bIncludeHidden = false;
	bIncludeResetable = true;
	bIncludeNestedPages = false;
}

/**
 * 设置搜索文本
 * 更新搜索表达式求值器，用于后续的文本匹配
 * @param InSearchText 要搜索的文本内容
 */
void FGameSettingFilterState::SetSearchText(const FString& InSearchText)
{
	// 设置搜索文本表达式，支持复杂的搜索语法
	SearchTextEvaluator.SetFilterText(FText::FromString(InSearchText));
}

/**
 * 检查设置项是否通过过滤器
 * 根据过滤器的各种条件判断设置项是否应该显示
 * @param InSetting 要检查的设置项
 * @return 是否通过过滤器
 */
bool FGameSettingFilterState::DoesSettingPassFilter(const UGameSetting& InSetting) const
{
	// 如果有允许列表且设置项不在其中，则不通过
	if (SettingAllowList.Num() > 0 && !IsSettingInAllowList(&InSetting))
	{
		return false;
	}

	// 检查搜索文本匹配
	if (!SearchTextEvaluator.GetFilterText().IsEmpty())
	{
		// 这里可以添加更复杂的搜索逻辑
		// 例如搜索设置项名称、描述等
		FString SettingName = InSetting.GetDevName().ToString();
		if (!SearchTextEvaluator.TestTextFilter(FBasicStringFilterExpressionContext(SettingName)))
		{
			return false;
		}
	}

	// 其他过滤条件可以在这里添加
	// 例如：bIncludeDisabled, bIncludeHidden 等的具体实现

	return true;
}

/**
 * 将设置项添加到根列表
 * @param InSetting 要添加的设置项
 */
void FGameSettingFilterState::AddSettingToRootList(UGameSetting* InSetting)
{
	if (InSetting && !SettingRootList.Contains(InSetting))
	{
		SettingRootList.Add(InSetting);
	}
}

/**
 * 将设置项添加到允许列表
 * @param InSetting 要添加的设置项
 */
void FGameSettingFilterState::AddSettingToAllowList(UGameSetting* InSetting)
{
	if (InSetting && !SettingAllowList.Contains(InSetting))
	{
		SettingAllowList.Add(InSetting);
	}
}

//////////////////////////////////////////////////////////////////////////
// UGameSetting

/**
 * 构造函数
 * 初始化游戏设置基类的默认值
 */
UGameSetting::UGameSetting()
{
	DevName = NAME_None;
	bStartupComplete = false;
}

/**
 * 获取子设置项列表
 * 基类默认返回空数组，子类可重写此方法返回实际的子设置项
 * @return 子设置项数组
 */
TArray<UGameSetting*> UGameSetting::GetChildSettings()
{
	return TArray<UGameSetting*>();
}

/**
 * 启动设置项
 * 在设置系统初始化时调用，进行基础的启动操作
 */
void UGameSetting::Startup()
{
	// 基类的启动逻辑
	// 子类可以重写此方法添加自定义启动逻辑
	
	// 标记启动完成
	bStartupComplete = true;
	
	// 调用初始化
	OnInitialized();
}

/**
 * 启动完成回调
 * 在所有设置项启动完成后调用
 */
void UGameSetting::StartupComplete()
{
	bStartupComplete = true;
}

/**
 * 初始化完成回调
 * 基类提供空实现，子类可重写进行自定义初始化
 */
void UGameSetting::OnInitialized()
{
	// 基类默认实现为空
	// 子类可以重写此方法进行自定义初始化
}

/**
 * 应用设置回调
 * 当设置被应用时调用，触发应用事件
 */
void UGameSetting::OnApply()
{
	// 触发设置应用事件
	OnSettingApplied.Broadcast(this);
}

/**
 * 依赖变更回调
 * 当依赖的设置项发生变化时调用
 */
void UGameSetting::OnDependencyChanged()
{
	// 基类默认实现为空
	// 子类可以重写此方法处理依赖变更逻辑
}

//////////////////////////////////////////////////////////////////////////
// UGameSettingValue

/**
 * 构造函数
 * 初始化值类型设置项的默认值
 */
UGameSettingValue::UGameSettingValue()
{
	// 调用基类构造函数
}

/**
 * 存储初始值
 * 保存设置项的初始状态，基类提供空实现
 * 子类应重写此方法保存具体的初始值
 */
void UGameSettingValue::StoreInitial()
{
	// 基类默认实现为空
	// 子类需要重写此方法保存具体的初始值
}

/**
 * 重置为默认值
 * 将设置项恢复为预设的默认值，基类提供空实现
 * 子类应重写此方法实现具体的重置逻辑
 */
void UGameSettingValue::ResetToDefault()
{
	// 基类默认实现为空
	// 子类需要重写此方法实现具体的重置逻辑
}


/**
 * 初始化完成回调
 * 重写基类方法，在初始化时自动存储初始值
 */
void UGameSettingValue::OnInitialized()
{
	Super::OnInitialized();
	
	// 自动存储初始值
	StoreInitial();
}

//////////////////////////////////////////////////////////////////////////
// UGameSettingValueDiscrete

/**
 * 构造函数
 * 初始化离散选择类型设置项的默认值
 */
// 在文件末尾添加 UGameSettingValueDiscrete 的实现

UGameSettingValueDiscrete::UGameSettingValueDiscrete()
{
	CurrentIndex = 0;
	DefaultIndex = 0;
}

FString UGameSettingValueDiscrete::GetAnalyticsValue() const
{
	return FString::Printf(TEXT("%d"), CurrentIndex);
}