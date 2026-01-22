#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"

class SMultiLineEditableTextBox;

/**
 * 资产描述编辑窗口类
 * 这是一个Slate窗口小部件，用于编辑UE资产的描述信息
 * 继承自SCompoundWidget，提供了一个多行文本编辑框和确认/取消按钮
 */
class CANCERASSETEDITOR_API SAssetDescriptionWindow : public SCompoundWidget
{
public:
    /**
     * Slate参数声明宏
     * 用于定义窗口创建时可以传入的参数
     * 在这个类中，我们不需要额外的参数，所以参数块是空的
     */
    SLATE_BEGIN_ARGS(SAssetDescriptionWindow)
    {}
    /** 是否启用逐个编辑模式 */
    SLATE_ARGUMENT(bool, bEnableSequentialEdit)
    SLATE_END_ARGS()

    /**
     * 构造函数
     * 用于构建窗口的UI界面，包括文本编辑框和按钮
     * @param InArgs - Slate参数结构，包含创建窗口时传入的参数
     */
    void Construct(const FArguments& InArgs);

    /**
     * 确认按钮点击事件处理函数
     * 当用户点击确认按钮时，会保存当前编辑的描述到所有选中的资产
     * @return FReply - Slate事件响应对象
     */
    FReply OnConfirmClicked();

    /**
     * 取消按钮点击事件处理函数
     * 当用户点击取消按钮时，会关闭窗口而不保存更改
     * @return FReply - Slate事件响应对象
     */
    FReply OnCancelClicked();

    /**
     * 下一个资产按钮点击事件处理函数
     * 保存当前资产的描述并切换到下一个资产
     * @return FReply - Slate事件响应对象
     */
    FReply OnNextAssetClicked();

    /**
     * 上一个资产按钮点击事件处理函数
     * 保存当前资产的描述并切换到上一个资产
     * @return FReply - Slate事件响应对象
     */
    FReply OnPreviousAssetClicked();

    /**
     * 获取当前文本编辑框中的描述文本
     * @return FString - 返回当前编辑框中的文本内容
     */
    FString GetCurrentDescription() const;

    /**
     * 设置要编辑的资产列表
     * 会将第一个资产的描述显示在编辑框中作为初始值
     * @param InAssets - 要编辑的资产数据数组
     */
    void SetAssetDescriptions(const TArray<FAssetData>& InAssets);

    /**
     * 获取当前正在编辑的资产名称
     * @return FText - 当前资产的显示名称
     */
    FText GetCurrentAssetText() const;

    /** 检查是否可以导航到下一个资产 */
    bool CanNavigateNext() const;

    /** 检查是否可以导航到上一个资产 */
    bool CanNavigatePrevious() const;

    /** 获取当前资产的BlueprintDescription */
    FText GetCurrentBlueprintDescription() const;

    /**
     * 编辑选中资产的描述
     * 这是一个可以在蓝图中调用的函数，会为当前选中的资产打开描述编辑窗口
     * 
     * @param bSequentialMode - 是否使用序列模式
     *        true: 序列模式，允许逐个编辑每个资产的描述
     *        false: 批量模式，所有选中的资产会被设置为相同的描述
     * 
     * 功能：
     * - 获取内容浏览器中当前选中的资产
     * - 创建并显示资产描述编辑窗口
     * - 根据模式提供不同的编辑体验：
     *   * 批量模式：所有资产使用相同描述
     *   * 序列模式：提供上一个/下一个按钮，每个资产可以有不同描述
     * 
     * 注意：
     * - 如果没有选中任何资产，函数会直接返回
     * - 在序列模式下，每个资产的描述会在切换时自动保存
     * - 对于蓝图资产，会修改其BlueprintDescription
     * - 对于其他类型的资产，会修改其类的Tooltip元数据
     */
    UFUNCTION(CallInEditor)
    void EditSelectedAssetsDescription(bool bSequentialMode = false);

private:
    /** 
     * 多行文本编辑框控件
     * 用于显示和编辑资产描述
     */
    TSharedPtr<SMultiLineEditableTextBox> DescriptionTextBox;

    /** 
     * 选中的资产列表
     * 存储所有需要编辑描述的资产
     */
    TArray<FAssetData> SelectedAssets;

    /**
     * 当前正在编辑的资产索引
     * 在逐个编辑模式下使用
     */
    int32 CurrentAssetIndex;

    /**
     * 是否启用逐个编辑模式
     * true: 一次编辑一个资产
     * false: 同时编辑所有资产
     */
    bool bEnableSequentialEdit;

    /**
     * 保存当前资产的描述
     * @return bool - 是否成功保存
     */
    bool SaveCurrentAssetDescription();

    /**
     * 加载指定索引的资产描述
     * @param AssetIndex - 要加载的资产索引
     */
    void LoadAssetDescription(int32 AssetIndex);

    /**
     * 更新窗口标题
     * 显示当前正在编辑的资产信息
     */
    void UpdateWindowTitle();
}; 