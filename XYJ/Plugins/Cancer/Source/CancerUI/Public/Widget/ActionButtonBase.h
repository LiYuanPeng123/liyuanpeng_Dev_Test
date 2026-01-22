#pragma once
#include "CancerUserWidget.h"
#include "Widget/CancerButtonBase.h"
#include "ActionButtonBase.generated.h"

UCLASS()
class CANCERUI_API UActionButtonBase:public UCancerUserWidget
{
    GENERATED_BODY()
    
public:
    UActionButtonBase(const FObjectInitializer& ObjectInitializer);
    
protected:
    virtual void NativeConstruct() override;
    
    // 按钮组件
    UPROPERTY(meta = (BindWidget))
    class UCancerButtonBase* ActionButton;
    
    // 按钮点击事件
    UFUNCTION()
    void OnActionButtonClicked();
    
    // 蓝图可重写的按钮点击事件
    UFUNCTION(BlueprintImplementableEvent, Category = "Action Button")
    void OnButtonClicked();
    
    // 设置按钮是否可用
    UFUNCTION(BlueprintCallable, Category = "Action Button")
    void SetButtonEnabled(bool bEnabled);
    
    // 获取按钮组件
    UFUNCTION(BlueprintPure, Category = "Action Button")
    UCancerButtonBase* GetActionButton() const { return ActionButton; }
    
    // 设置按钮交互状态
    UFUNCTION(BlueprintCallable, Category = "Action Button")
    void SetInteractionEnabled(bool bEnabled);
    
    // 设置按钮焦点状态
    UFUNCTION(BlueprintCallable, Category = "Action Button")
    void SetButtonFocusable(bool bFocusable);
    
    // 检查按钮是否被悬停
    UFUNCTION(BlueprintPure, Category = "Action Button")
    bool IsButtonHovered() const;
    
    // 检查按钮是否被按下
    UFUNCTION(BlueprintPure, Category = "Action Button")
    bool IsButtonPressed() const;
    
    // 双击事件处理
    UFUNCTION()
    void OnActionButtonDoubleClicked();
    
    // 蓝图可重写的双击事件
    UFUNCTION(BlueprintImplementableEvent, Category = "Action Button")
    void OnButtonDoubleClicked();
};
