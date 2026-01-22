#include "CancerCamera.h"

#define LOCTEXT_NAMESPACE "FCancerCameraModule"

void FCancerCameraModule::StartupModule()
{
    // 模块启动逻辑
}

void FCancerCameraModule::ShutdownModule()
{
    // 模块关闭逻辑
}

#undef LOCTEXT_NAMESPACE

DEFINE_LOG_CATEGORY(LogCancerCamera);
IMPLEMENT_MODULE(FCancerCameraModule, CancerCamera)