#include "Editor/ProjectilePreviewEdMode.h"
#include "CancerMontageTools/Public/Editor/ProjectilePreviewAPI.h"
#include "EditorModeRegistry.h"
#include "EditorModeManager.h"
#include "Engine/World.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

static TWeakObjectPtr<AActor> GCancerPreviewHandle;
static TWeakObjectPtr<UClass> GCancerPlaceClass;
static bool GCancerPreviewModeRegistered = false;
static TWeakObjectPtr<AActor> GCancerSelectedActor;

const FEditorModeID FCancerProjectilePreviewEdMode::EM_CancerProjectilePreviewEdMode = FEditorModeID(TEXT("EM_CancerProjectilePreviewEdMode"));

FCancerProjectilePreviewEdMode::FCancerProjectilePreviewEdMode() {}

void FCancerProjectilePreviewEdMode::Enter()
{
    FEdMode::Enter();
    GLevelEditorModeTools().SetWidgetMode(UE::Widget::WM_None);
}

void FCancerProjectilePreviewEdMode::Exit()
{
    bDragging = false;
    FEdMode::Exit();
}

bool FCancerProjectilePreviewEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
    if (Key == EKeys::LeftMouseButton)
    {
        if (Event == IE_Pressed)
        {
            bDragging = true;
            FIntPoint MousePos; Viewport->GetMousePos(MousePos);
            FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, ViewportClient->GetScene(), ViewportClient->EngineShowFlags));
            FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);
            if (SceneView)
            {
                FVector WorldOrigin, WorldDirection; 
                SceneView->DeprojectFVector2D(FVector2D(MousePos), WorldOrigin, WorldDirection);
                AActor* Best = nullptr;
                float BestDist = TNumericLimits<float>::Max();
                if (GEngine)
                {
                    for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
                    {
                        UWorld* World = Ctx.World();
                        if (World && World->IsPreviewWorld())
                        {
                            for (TActorIterator<AActor> It(World); It; ++It)
                            {
                                AActor* A = *It;
                                const FVector P = A->GetActorLocation();
                                const FVector AP = P - WorldOrigin;
                                const float T = FVector::DotProduct(AP, WorldDirection);
                                const FVector Closest = WorldOrigin + WorldDirection * T;
                                const float Dist = FVector::Dist(Closest, P);
                                if (Dist < 50.f && Dist < BestDist)
                                {
                                    Best = A;
                                    BestDist = Dist;
                                }
                            }
                            break;
                        }
                    }
                }
                if (Best)
                {
                    GCancerSelectedActor = Best;
                    GCancerPreviewHandle = Best;
                }
            }
        }
        if (Event == IE_Released)
        {
            bDragging = false;
            if (GCancerPlaceClass.IsValid())
            {
                FIntPoint MousePos; Viewport->GetMousePos(MousePos);
                FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, ViewportClient->GetScene(), ViewportClient->EngineShowFlags));
                FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);
                if (SceneView)
                {
                    FVector WorldOrigin, WorldDirection; 
                    SceneView->DeprojectFVector2D(FVector2D(MousePos), WorldOrigin, WorldDirection);
                    const FVector PlaneOrigin = FVector::ZeroVector; 
                    const FVector PlaneNormal = FVector::UpVector;
                    const float Den = FVector::DotProduct(WorldDirection, PlaneNormal);
                    FVector SpawnPoint = WorldOrigin;
                    if (!FMath::IsNearlyZero(Den))
                    {
                        const float T = FVector::DotProduct(PlaneOrigin - WorldOrigin, PlaneNormal) / Den;
                        SpawnPoint = WorldOrigin + WorldDirection * T;
                    }
                    if (GEngine)
                    {
                        for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
                        {
                            UWorld* World = Ctx.World();
                            if (World && World->IsPreviewWorld())
                            {
                                World->SpawnActor<AActor>(GCancerPlaceClass.Get(), SpawnPoint, FRotator::ZeroRotator);
                                break;
                            }
                        }
                    }
                }
                GCancerPlaceClass.Reset();
            }
        }
        return true;
    }
    return FEdMode::InputKey(ViewportClient, Viewport, Key, Event);
}

void FCancerProjectilePreviewEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
    FEdMode::Tick(ViewportClient, DeltaTime);
    if (!bDragging || !GCancerPreviewHandle.IsValid())
    {
        return;
    }
    FViewport* VP = ViewportClient ? ViewportClient->Viewport : nullptr;
    if (!VP)
    {
        return;
    }
    FIntPoint MousePos; VP->GetMousePos(MousePos);
    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(VP, ViewportClient->GetScene(), ViewportClient->EngineShowFlags));
    FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);
    if (!SceneView)
    {
        return;
    }
    FVector WorldOrigin, WorldDirection;
    SceneView->DeprojectFVector2D(FVector2D(MousePos), WorldOrigin, WorldDirection);
    const FVector PlaneOrigin = GCancerPreviewHandle->GetActorLocation();
    const FVector PlaneNormal = FVector::UpVector;
    const float T = FVector::DotProduct(PlaneOrigin - WorldOrigin, PlaneNormal) / FVector::DotProduct(WorldDirection, PlaneNormal);
    const FVector HitPoint = WorldOrigin + WorldDirection * T;
    GCancerPreviewHandle->SetActorLocation(HitPoint);
}

void SetCancerPreviewHandle(AActor* InHandle)
{
    GCancerPreviewHandle = InHandle;
    if (!GCancerPreviewModeRegistered)
    {
        FEditorModeRegistry::Get().RegisterMode<FCancerProjectilePreviewEdMode>(FCancerProjectilePreviewEdMode::EM_CancerProjectilePreviewEdMode, FText::FromString(TEXT("Projectile Preview")), FSlateIcon(), true);
        GCancerPreviewModeRegistered = true;
    }
    GLevelEditorModeTools().ActivateMode(FCancerProjectilePreviewEdMode::EM_CancerProjectilePreviewEdMode);
    GLevelEditorModeTools().SetWidgetMode(UE::Widget::WM_None);
}

void StartCancerDragPlace(UClass* InClass)
{
    GCancerPlaceClass = InClass;
    if (!GCancerPreviewModeRegistered)
    {
        FEditorModeRegistry::Get().RegisterMode<FCancerProjectilePreviewEdMode>(FCancerProjectilePreviewEdMode::EM_CancerProjectilePreviewEdMode, FText::FromString(TEXT("Projectile Preview")), FSlateIcon(), true);
        GCancerPreviewModeRegistered = true;
    }
    GLevelEditorModeTools().ActivateMode(FCancerProjectilePreviewEdMode::EM_CancerProjectilePreviewEdMode);
    GLevelEditorModeTools().SetWidgetMode(UE::Widget::WM_None);
}
