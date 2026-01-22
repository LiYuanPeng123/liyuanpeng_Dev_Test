
#pragma once
#include "CoreMinimal.h"


/**
 * 角色移动方向属性的自定义编辑器类
 * 该类用于在Unreal Engine的属性面板中自定义移动方向属性的编辑界面
 */
class CANCERANIMATIONEDITOR_API FLocomotionDirectionCustomization : public IPropertyTypeCustomization
{
public:
	/**
 * 创建自定义编辑器实例的工厂方法
 * @return 返回一个新的FLocomotionDirectionCustomization实例
 */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FLocomotionDirectionCustomization);
	}
	
	/**
	 * 自定义属性头的显示
	 * @param PropertyHandle 要自定义的属性句柄
	 * @param HeaderRow 属性头部的显示行
	 * @param Utils 自定义工具实用程序
	 */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& Utils) override;
	FText GetModeText() const;

	/**
	 * 自定义属性的子属性显示
	 * @param PropertyHandle 主属性句柄
	 * @param ChildBuilder 子属性构建器
	 * @param Utils 自定义工具实用程序
	 */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& Utils) override;
private:
	
	/**
	 * 获取属性列表
	 * @param PropertyHandle 主属性句柄
	 * @return 属性句柄数组
	 */
	TArray<TSharedPtr<IPropertyHandle>> GetList(TSharedRef<IPropertyHandle> PropertyHandle) const;
	
	/** 主属性句柄 */
	TSharedPtr<IPropertyHandle> OwnerPropertyHandle;
	
	/** 属性工具实用程序 */
	TSharedPtr<IPropertyUtilities> AUtils;
	
	/** 强制刷新属性 */
	void ForceRefresh();
	FReply OnRefreshClicked();

	/** 获取错误资源数量 */
	int32 GetErrorAssetCount() const;
	
	/** 获取数据有效性文本 */
	FText GetDataValidText() const;
	
	/** 获取数据有效性显示状态 */
	EVisibility GetDataValidVisibility() const;

};
