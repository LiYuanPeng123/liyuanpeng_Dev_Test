// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/Widget.h"

#include "IndicatorLayer.generated.h"

class SActorCanvas;
class SWidget;
class UObject;

UCLASS()
class UIndicatorLayer : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** Default arrow brush to use if UI is clamped to the screen and needs to show an arrow. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush ArrowBrush; // 屏幕钉边箭头画刷

protected:
	// UWidget interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override; // 释放画布资源
	virtual TSharedRef<SWidget> RebuildWidget() override; // 构建 SActorCanvas
	// End UWidget

protected:
	TSharedPtr<SActorCanvas> MyActorCanvas; // 画布实例
};
