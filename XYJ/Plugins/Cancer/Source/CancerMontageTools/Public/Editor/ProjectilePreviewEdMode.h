#pragma once

#include "EditorModeManager.h"

class FCancerProjectilePreviewEdMode : public FEdMode
{
public:
    static const FEditorModeID EM_CancerProjectilePreviewEdMode;

    FCancerProjectilePreviewEdMode();
    virtual ~FCancerProjectilePreviewEdMode() override {}

    virtual void Enter() override;
    virtual void Exit() override;
    virtual bool UsesToolkits() const override { return false; }
    virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
    virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;

private:
    bool bDragging = false;
};

