#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "AttributeSet.h"
#include "Styling/SlateBrush.h"
#include "CancerAttributeBarWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;

/**
 *   UI 进度条控件
 * - 通过 AbilitySystem 的属性变更回调实时更新
 */
UCLASS(BlueprintType, Blueprintable)
class CANCERUI_API UCancerAttributeBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCancerAttributeBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 设置数据来源的角色（从该 Actor 解析 AbilitySystemComponent）
	UFUNCTION(BlueprintCallable, Category="Cancer|UI|Attribute")
	void SetSourceActor(AActor* InActor);

	// 主动刷新一次（当外部手动修改数据或需要强制同步时可用）
	UFUNCTION(BlueprintCallable, Category="Cancer|UI|Attribute")
	void Refresh();

protected:
	// 在编辑器预览与运行前应用外观配置
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 绑定 ASC 属性变化回调
	void BindToAttributes();
	void UnbindFromAttributes();

	void HandleAttributeChanged(const FOnAttributeChangeData& Data);
	void UpdatePercent();

	UAbilitySystemComponent* ResolveASC() const;

	// 立即应用外观设置（背景/填充 图片与颜色）。在编辑器中调用可实时预览
	UFUNCTION(BlueprintCallable, Category="Cancer|UI|Attribute|Appearance")
	void ApplyAppearance();

protected:
	// 绑定到的进度条（在蓝图派生类中通过同名控件自动绑定）
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess="true"))
	UProgressBar* AttributeProgress = nullptr;

	// 绑定到的文本显示：显示 "当前/最大" 数值（可选）
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess="true"))
	UTextBlock* AttributeText = nullptr;

	// 可配置：当前生命属性（默认 UCancerAttributeSet::Attribute）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute")
	FGameplayAttribute CurrentAttribute;

	// 可配置：最大生命属性（默认 UCancerAttributeSet::MaxAttribute）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute")
	FGameplayAttribute MaxAttribute;

	// 是否在构建时自动从拥有者（GetOwningPlayerPawn 或 GetOwningPlayer）解析 ASC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute")
	bool bAutoBindOwnerASC = true;

	// 外部指定的数据来源 Actor（优先级高于自动解析）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute")
	TWeakObjectPtr<AActor> SourceActor;

	// 外观：背景与填充的 Brush（可配置图片与 Tint），以及填充颜色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|Appearance")
	FSlateBrush ProgressBackgroundBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|Appearance")
	FSlateBrush ProgressFillBrush;

	// 进度条填充颜色（与 ProgressFillBrush 的 Tint 叠加）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|Appearance")
	FLinearColor ProgressFillColor = FLinearColor(0.0f, 0.8f, 0.2f, 1.0f);

	// 是否启用低血量高亮（小于阈值时切换颜色）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|LowAttribute")
	bool bEnableLowAttributeColor = false;

	// 低血量阈值（0-1 的百分比），仅当 bEnableLowAttributeColor=true 时生效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|LowAttribute", meta=(ClampMin="0.0", ClampMax="1.0"))
	float LowAttributeThreshold = 0.3f;

	// 正常与低血量时的进度条颜色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|LowAttribute")
	FLinearColor NormalBarColor = FLinearColor(0.0f, 0.8f, 0.2f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|LowAttribute")
	FLinearColor LowBarColor = FLinearColor(0.9f, 0.1f, 0.1f, 1.0f);

	// 正常与低血量时的文本颜色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|LowAttribute")
	FLinearColor NormalTextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer|UI|Attribute|LowAttribute")
	FLinearColor LowTextColor = FLinearColor(1.0f, 0.2f, 0.2f, 1.0f);

private:
	// 缓存的 ASC
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> CachedASC = nullptr;

	// 变更委托句柄，用于解绑
	FDelegateHandle CurrentChangedHandle;
	FDelegateHandle MaxChangedHandle;
};
