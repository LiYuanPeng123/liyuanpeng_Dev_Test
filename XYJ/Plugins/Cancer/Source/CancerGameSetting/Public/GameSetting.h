#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "GameSetting.generated.h"

class UGameSetting;
/**
 * 设置变更原因枚举
 * 用于标识设置项发生变化的具体原因，便于UI和逻辑系统做出相应响应
 */
UENUM(BlueprintType)
enum class EGameSettingChangeReason : uint8
{
	Change,              // 用户主动变化（如拖动滑块、选择选项等）
	DependencyChanged,   // 依赖关系更改（其他设置项的变化影响了当前设置）
	ResetToDefault,      // 恢复默认设置（点击重置按钮）
	RestoreToInitial,    // 恢复至初始状态（取消更改，恢复到打开设置界面时的状态）
};

/**
 * 设置过滤器状态结构体
 * 用于控制设置项的显示和筛选逻辑，支持搜索文本、显示条件等
 */
USTRUCT(BlueprintType)
struct CANCERGAMESETTING_API FGameSettingFilterState
{
	GENERATED_BODY()

public:
	FGameSettingFilterState();

	/** 是否包含被禁用的设置项 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "过滤器", meta = (DisplayName = "包含禁用项"))
	bool bIncludeDisabled = true;

	/** 是否包含隐藏的设置项 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "过滤器", meta = (DisplayName = "包含隐藏项"))
	bool bIncludeHidden = false;

	/** 是否包含可重置的设置项 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "过滤器", meta = (DisplayName = "包含可重置项"))
	bool bIncludeResetable = true;

	/** 是否包含嵌套页面的设置项 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "过滤器", meta = (DisplayName = "包含嵌套页面"))
	bool bIncludeNestedPages = false;

public:
	/**
	 * 设置搜索文本
	 * @param InSearchText 要搜索的文本内容
	 */
	void SetSearchText(const FString& InSearchText);

	/**
	 * 检查设置项是否通过过滤器
	 * @param InSetting 要检查的设置项
	 * @return 是否通过过滤器
	 */
	bool DoesSettingPassFilter(const UGameSetting& InSetting) const;

	/**
	 * 将设置项添加到根列表
	 * @param InSetting 要添加的设置项
	 */
	void AddSettingToRootList(UGameSetting* InSetting);

	/**
	 * 将设置项添加到允许列表
	 * @param InSetting 要添加的设置项
	 */
	void AddSettingToAllowList(UGameSetting* InSetting);

	/**
	 * 检查设置项是否在允许列表中
	 * @param InSetting 要检查的设置项
	 * @return 是否在允许列表中
	 */
	bool IsSettingInAllowList(const UGameSetting* InSetting) const
	{
		return SettingAllowList.Contains(InSetting);
	}
	
	/**
	 * 获取设置根列表
	 * @return 设置根列表的常量引用
	 */
	const TArray<UGameSetting*>& GetSettingRootList() const { return SettingRootList; }

	/**
	 * 检查设置项是否在根列表中
	 * @param InSetting 要检查的设置项
	 * @return 是否在根列表中
	 */
	bool IsSettingInRootList(const UGameSetting* InSetting) const
	{
		return SettingRootList.Contains(InSetting);
	}

private:
	/** 搜索文本表达式求值器，用于复杂搜索逻辑 */
	FTextFilterExpressionEvaluator SearchTextEvaluator;

	/** 设置项根列表，包含顶级设置项 */
	UPROPERTY()
	TArray<TObjectPtr<UGameSetting>> SettingRootList;

	/** 设置项允许列表，如果非空则只显示列表中的设置项 */
	UPROPERTY()
	TArray<TObjectPtr<UGameSetting>> SettingAllowList;
};

/**
 * 游戏设置基类
 * 所有游戏设置项的抽象基类，提供基础的设置管理功能
 * 包括设置名称管理、事件系统、初始化流程等
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CANCERGAMESETTING_API UGameSetting : public UObject
{
	GENERATED_BODY()

public:
	UGameSetting();


	DECLARE_EVENT_TwoParams(UGameSetting, FOnSettingChanged, UGameSetting* /*InSetting*/, EGameSettingChangeReason /*InChangeReason*/);
	DECLARE_EVENT_OneParam(UGameSetting, FOnSettingApplied, UGameSetting* /*InSetting*/);

	/** 设置变更事件 */
	FOnSettingChanged OnSettingChanged;

	/** 设置应用事件 */
	FOnSettingApplied OnSettingApplied;

	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual void RestoreToInitial() {}
	
	/**
	 * 设置父级设置项
	 * @param InParent 父级设置项
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetParentSetting(UGameSetting* InParent) { ParentSetting = InParent; }

	/**
	 * 获取父级设置项
	 * @return 父级设置项
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	UGameSetting* GetParentSetting() const { return ParentSetting; }

	/**
		 * 获取用于分析的值
		 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	virtual FString GetAnalyticsValue() const { return TEXT("");};

	/**
	 * 获取设置项的开发者名称（唯一标识符）
	 * @return 设置项的开发者名称
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取开发者名称"))
	FName GetDevName() const { return DevName; }

	/**
	 * 设置开发者名称
	 * @param Value 新的开发者名称
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "设置开发者名称"))
	void SetDevName(const FName& Value) { DevName = Value; }

	/**
	 * 获取子设置项列表
	 * 对于设置集合类型，返回包含的子设置项
	 * @return 子设置项数组
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取子设置项"))
	virtual TArray<UGameSetting*> GetChildSettings();

	/**
	 * 启动设置项
	 * 在设置系统初始化时调用，用于设置项的基础初始化
	 */
	virtual void Startup();

	/**
	 * 启动完成回调
	 * 在所有设置项启动完成后调用
	 */
	void StartupComplete();

	/**
	 * 初始化完成回调
	 * 在设置项初始化完成时调用，子类可重写此方法进行自定义初始化
	 */
	virtual void OnInitialized();

	/**
	 * 应用设置回调
	 * 当设置被应用时调用，子类可重写此方法进行自定义应用逻辑
	 */
	virtual void OnApply();

	/**
	 * 依赖变更回调
	 * 当依赖的设置项发生变化时调用
	 */
	virtual void OnDependencyChanged();
	// 在 UGameSetting 类中添加缺失的方法：

	/**
	 * 设置显示名称
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetDisplayName(const FText& InDisplayName) { DisplayName = InDisplayName; }

	/**
	 * 获取显示名称
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	FText GetDisplayName() const { return DisplayName; }

	/**
	 * 设置描述文本
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	void SetDescriptionRichText(const FText& InDescription) { DescriptionRichText = InDescription; }

	/**
	 * 获取描述文本
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "设置")
	FText GetDescriptionRichText() const { return DescriptionRichText; }

protected:
	/** 显示名称 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	FText DisplayName;

	/** 描述文本 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	FText DescriptionRichText;

protected:
	/** 设置项的开发者名称，用作唯一标识符 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置", meta = (DisplayName = "开发者名称"))
	FName DevName;

	/** 是否已完成启动 */
	bool bStartupComplete = false;
	
	/** 父级设置项 */
	UPROPERTY()
	TObjectPtr<UGameSetting> ParentSetting;
};

/**
 * 值类型设置项基类
 * 继承自UGameSetting，为具有值的设置项提供基础功能
 * 包括初始值存储、重置、恢复等操作
 */
UCLASS(Abstract, BlueprintType)
class CANCERGAMESETTING_API UGameSettingValue : public UGameSetting
{
	GENERATED_BODY()

public:
	UGameSettingValue();

	/**
	 * 存储初始值
	 * 保存设置项的初始状态，用于后续的恢复操作
	 * 在初始化时调用，也可在应用设置后调用
	 */
	virtual void StoreInitial();

	/**
	 * 重置为默认值
	 * 将设置项恢复为预设的默认值
	 */
	virtual void ResetToDefault();

	
	/**
	 * 恢复到初始值
	 * 将设置项恢复为打开设置界面时的初始状态
	 */
	virtual void RestoreToInitial() override {}
	
	/**
	 * 应用设置
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual void Apply() {}

protected:
	/**
	 * 初始化完成回调
	 * 重写基类方法，在初始化时自动存储初始值
	 */
	virtual void OnInitialized() override;
};

/**
 * 离散选择类型设置项基类
 * 继承自UGameSettingValue，用于实现下拉选择、单选按钮等离散选择类型的设置
 * 提供选项管理和索引操作的基础功能
 */
UCLASS(Abstract, BlueprintType)
class CANCERGAMESETTING_API UGameSettingValueDiscrete : public UGameSettingValue
{
	GENERATED_BODY()

public:
	UGameSettingValueDiscrete();

	/**
	 * 设置选项列表
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual void SetOptions(const TArray<FText>& InOptions) { Options = InOptions; }

	/**
	 * 设置默认选项索引
	 */
	UFUNCTION(BlueprintCallable, Category = "设置")
	virtual void SetDefaultValueFromIndex(int32 Index) { DefaultIndex = Index; CurrentIndex = Index; }

	/**
	 * 通过索引设置离散选项
	 * @param Index 要设置的选项索引
	 */
	// 在 UGameSettingValueDiscrete 类中确保这个方法触发事件
	virtual void SetDiscreteOptionByIndex(int32 Index) 
	{ 
	if (CurrentIndex != Index)
	{
		CurrentIndex = FMath::Clamp(Index, 0, FMath::Max(0, Options.Num() - 1)); 
		// 触发设置变更事件
		OnSettingChanged.Broadcast(this, EGameSettingChangeReason::Change);
	}
	}

	/**
	 * 获取当前选中的离散选项索引
	 * @return 当前选中的选项索引
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取选项索引"))
	virtual int32 GetDiscreteOptionIndex() const { return CurrentIndex; }

	/**
	 * 获取默认的离散选项索引
	 * @return 默认选项索引
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取默认选项索引"))
	virtual int32 GetDiscreteOptionDefaultIndex() const { return DefaultIndex; }

	/**
	 * 获取所有离散选项
	 * @return 所有可选项的文本数组
	 */
	UFUNCTION(BlueprintCallable, Category = "设置", meta = (DisplayName = "获取所有选项"))
	virtual TArray<FText> GetDiscreteOptions() const { return Options; }

	/**
	 * 获取用于分析的值
	 * 返回用于数据分析和统计的字符串表示
	 * @return 分析用的字符串值
	 */
	virtual FString GetAnalyticsValue() const;
	
	virtual void RestoreToInitial() override { SetDiscreteOptionByIndex(DefaultIndex);}

protected:
	/** 选项列表 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	TArray<FText> Options;

	/** 当前选中的索引 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	int32 CurrentIndex = 0;

	/** 默认索引 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "设置")
	int32 DefaultIndex = 0;
};




