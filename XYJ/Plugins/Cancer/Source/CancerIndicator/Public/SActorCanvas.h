
#pragma once

#include "AsyncMixin.h"
#include "Blueprint/UserWidgetPool.h"
#include "Widgets/SPanel.h"

class FActiveTimerHandle;
class FArrangedChildren;
class FChildren;
class FPaintArgs;
class FReferenceCollector;
class FSlateRect;
class FSlateWindowElementList;
class FWidgetStyle;
class UIndicatorDescriptor;
class UCancerIndicatorManagerComponent;
struct FSlateBrush;

class SActorCanvas : public SPanel, public FAsyncMixin, public FGCObject
{
public:
	/** ActorCanvas-specific slot class */
	class FSlot : public TSlotBase<FSlot>
	{
	public:
		FSlot(UIndicatorDescriptor* InIndicator)
			: TSlotBase<FSlot>()
			  , Indicator(InIndicator)
			  , ScreenPosition(FVector2D::ZeroVector)
			  , Depth(0)
			  , Priority(0.f)
			  , bIsIndicatorVisible(true)
			  , bInFrontOfCamera(true)
			  , bHasValidScreenPosition(false)
			  , bDirty(true)
			  , bWasIndicatorClamped(false)
			  , bWasIndicatorClampedStatusChanged(false)
		{
		}

		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
		SLATE_SLOT_END_ARGS()

		using TSlotBase<FSlot>::Construct;

		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }

		void SetIsIndicatorVisible(bool bVisible)
		{
			if (bIsIndicatorVisible != bVisible)
			{
				bIsIndicatorVisible = bVisible;
				bDirty = true;
			}

			RefreshVisibility();
		}

		FVector2D GetScreenPosition() const { return ScreenPosition; }

		void SetScreenPosition(FVector2D InScreenPosition)
		{
			if (ScreenPosition != InScreenPosition)
			{
				ScreenPosition = InScreenPosition;
				bDirty = true;
			}
		}

		double GetDepth() const { return Depth; }

		void SetDepth(double InDepth)
		{
			if (Depth != InDepth)
			{
				Depth = InDepth;
				bDirty = true;
			}
		}

		int32 GetPriority() const { return Priority; }

		void SetPriority(int32 InPriority)
		{
			if (Priority != InPriority)
			{
				Priority = InPriority;
				bDirty = true;
			}
		}

		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }

		void SetInFrontOfCamera(bool bInFront)
		{
			if (bInFrontOfCamera != bInFront)
			{
				bInFrontOfCamera = bInFront;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }

		void SetHasValidScreenPosition(bool bValidScreenPosition)
		{
			if (bHasValidScreenPosition != bValidScreenPosition)
			{
				bHasValidScreenPosition = bValidScreenPosition;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool bIsDirty() const { return bDirty; }

		void ClearDirtyFlag()
		{
			bDirty = false;
		}

		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }

		void SetWasIndicatorClamped(bool bWasClamped) const
		{
			if (bWasClamped != bWasIndicatorClamped)
			{
				bWasIndicatorClamped = bWasClamped;
				bWasIndicatorClampedStatusChanged = true;
			}
		}

		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }

		void ClearIndicatorClampedStatusChangedFlag()
		{
			bWasIndicatorClampedStatusChanged = false;
		}

	private:
		void RefreshVisibility()
		{
			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;
			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
		}

		//Kept Alive by SActorCanvas::AddReferencedObjects
		UIndicatorDescriptor* Indicator; // 指示器描述器
		FVector2D ScreenPosition; // 屏幕位置
		double Depth; // 深度（用于排序）
		int32 Priority; // 优先级（用于排序）

		uint8 bIsIndicatorVisible : 1; // 是否可见
		uint8 bInFrontOfCamera : 1; // 是否在相机前
		uint8 bHasValidScreenPosition : 1; // 屏幕位置是否有效
		uint8 bDirty : 1; // 是否需要重绘

		/** 
		 * Cached & frame-deferred value of whether the indicator was visually screen clamped last frame or not; 
		 * Semi-hacky mutable implementation as it is cached during a const paint operation
		 */
		mutable uint8 bWasIndicatorClamped : 1; // 上一帧是否被钉边
		mutable uint8 bWasIndicatorClampedStatusChanged : 1; // 钉边状态是否发生变化

		friend class SActorCanvas;
		};

	/** ActorCanvas-specific slot class */
	class FArrowSlot : public TSlotBase<FArrowSlot>
	{
	};

	/** Begin the arguments for this slate widget */
	SLATE_BEGIN_ARGS(SActorCanvas)
		{
			_Visibility = EVisibility::HitTestInvisible;
		}

		/** Indicates that we have a slot that this widget supports */
		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)

		/** This always goes at the end */
	SLATE_END_ARGS()

	SActorCanvas()
		: CanvasChildren(this)
		  , ArrowChildren(this)
		  , AllChildren(this)
	{
		AllChildren.AddChildren(CanvasChildren);
		AllChildren.AddChildren(ArrowChildren);
	}

	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InCtx,
	               const FSlateBrush* ActorCanvasArrowBrush);

	// SWidget Interface
	virtual void
	OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }
	virtual FChildren* GetChildren() override { return &AllChildren; }
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const;
	// End SWidget

	void SetDrawElementsInOrder(bool bInDrawElementsInOrder) { bDrawElementsInOrder = bInDrawElementsInOrder; }

	virtual FString GetReferencerName() const override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	void OnIndicatorAdded(UIndicatorDescriptor* Indicator); // 指示器添加回调
	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator); // 指示器移除回调

	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator); // 异步加载并加入池与画布
	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator); // 解绑并释放控件

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	FScopedWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator); // 添加画布槽位
	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget); // 移除画布槽位

	void SetShowAnyIndicators(bool bIndicators); // 控制整体可见性
	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime); // 定时更新投影与布局

	/** Helper function for calculating the offset */
	void GetOffsetAndSize(const UIndicatorDescriptor* Indicator,
	                      FVector2D& OutSize,
	                      FVector2D& OutOffset,
	                      FVector2D& OutPaddingMin,
	                      FVector2D& OutPaddingMax) const; // 计算对齐与偏移

	void UpdateActiveTimer(); // 根据状态启停定时器

private:
	TArray<TObjectPtr<UIndicatorDescriptor>> AllIndicators; // 所有指示器
	TArray<UIndicatorDescriptor*> InactiveIndicators; // 待激活指示器

	FLocalPlayerContext LocalPlayerContext; // 本地玩家上下文
	TWeakObjectPtr<UCancerIndicatorManagerComponent> IndicatorComponentPtr; // 管理组件指针

	/** All the slots in this canvas */
	TPanelChildren<FSlot> CanvasChildren; // 指示器槽集合
	mutable TPanelChildren<FArrowSlot> ArrowChildren; // 箭头槽集合
	FCombinedChildren AllChildren; // 合并槽集合

	FUserWidgetPool IndicatorPool; // 控件对象池

	const FSlateBrush* ActorCanvasArrowBrush = nullptr; // 箭头画刷引用

	mutable int32 NextArrowIndex = 0; // 下一可用箭头索引
	mutable int32 ArrowIndexLastUpdate = 0; // 上次更新的箭头数量

	/** Whether to draw elements in the order they were added to canvas. Note: Enabling this will disable batching and will cause a greater number of drawcalls */
	bool bDrawElementsInOrder = false; // 是否按添加顺序绘制

	bool bShowAnyIndicators = false; // 当前是否显示任意指示器

	mutable TOptional<FGeometry> OptionalPaintGeometry; // 最近一次绘制几何

	TSharedPtr<FActiveTimerHandle> TickHandle; // 活动计时器句柄
};
