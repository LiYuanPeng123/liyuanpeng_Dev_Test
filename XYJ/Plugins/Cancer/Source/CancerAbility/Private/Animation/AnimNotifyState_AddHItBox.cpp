// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_AddHItBox.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

TAutoConsoleVariable<bool> HitBoxVar(TEXT("Cancer.DynamicHitBox"), false,TEXT(""));

void UAnimNotifyState_AddHItBox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (SpawnedBox)
	{
		SpawnedBox->DestroyComponent();
		SpawnedBox = nullptr;
	}

	if (MeshComp && MeshComp->GetWorld() && !MeshComp->GetWorld()->IsPreviewWorld())
	{
		if (AActor* OwnerActor = MeshComp->GetOwner())
		{
			const FVector Extent = HitBox.IsNearlyZero() ? FVector(30.f, 30.f, 30.f) : HitBox;
			if (UBoxComponent* BoxComp = NewObject<UBoxComponent>(OwnerActor))
			{
				BoxComp->SetBoxExtent(Extent);
				BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				BoxComp->SetGenerateOverlapEvents(true);
				BoxComp->SetCollisionObjectType(Channel);
				BoxComp->SetMobility(EComponentMobility::Movable);
				BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				BoxComp->RegisterComponent();
				if (!AttachName.IsNone())
				{
					BoxComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					                           AttachName);
				}
				else
				{
					BoxComp->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepRelativeTransform);
				}

				// 应用位置偏移（局部空间）
				BoxComp->SetRelativeLocation(HitBoxOffset);
				SpawnedBox = BoxComp;
			}
		}
	}
}

void UAnimNotifyState_AddHItBox::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (SpawnedBox)
	{
		SpawnedBox->SetHiddenInGame(HitBoxVar.GetValueOnGameThread());
	}


#if WITH_EDITOR
	// 仅在编辑器预览中绘制碰撞盒，方便美术/设计校准
	if (MeshComp && MeshComp->GetWorld() && MeshComp->GetWorld()->IsPreviewWorld())
	{
		const FName SocketName = AttachName.IsNone() ? NAME_None : AttachName;
		FVector Location = SocketName.IsNone()
			                   ? MeshComp->GetComponentLocation()
			                   : MeshComp->GetSocketLocation(SocketName);
		FRotator Rotation = SocketName.IsNone()
			                    ? MeshComp->GetComponentRotation()
			                    : MeshComp->GetSocketRotation(SocketName);
		// 偏移为局部空间，按旋转变换后叠加
		Location += FQuat(Rotation).RotateVector(HitBoxOffset);

		const FVector Extent = HitBox.IsNearlyZero() ? FVector(30.f, 30.f, 30.f) : HitBox;

		DrawDebugBox(
			MeshComp->GetWorld(),
			Location,
			Extent,
			FQuat(Rotation),
			FColor::Red,
			false,
			-1.f,
			0,
			1.f
		);
	}
#endif
}

void UAnimNotifyState_AddHItBox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (SpawnedBox)
	{
		SpawnedBox->DestroyComponent();
		SpawnedBox = nullptr;
	}
}

void UAnimNotifyState_AddHItBox::HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::HandleTimeExpired(MeshComp, Animation);
	
}
