#include "Components/CancerLocomotionComponent.h"

#include "CancerAssetManager.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(CancerLocomotionLog)

void UCancerLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	check(OwnerCharacter.Get());
	OwnerMeshComponent = OwnerCharacter.Get()->GetMesh();

	if (OwnerMeshComponent->GetAnimationMode() != EAnimationMode::Type::AnimationBlueprint)
	{
		OwnerMeshComponent->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	}
	//设置动画蓝图类
	OwnerMeshComponent->SetAnimInstanceClass(AnimInstanceClass);
	//链接默认动画层
	SetCurrentAnimLink(DefaultLinkTag);
}

void UCancerLocomotionComponent::InitializeComponent()
{
	Super::InitializeComponent();
}


void UCancerLocomotionComponent::SetCurrentAnimLink(const FGameplayTag& InTag)
{
	if (FLocomotion* Locomotion = LinkAnimMap.Find(InTag))
	{
		TSubclassOf<UAnimInstance> AnimIns = UCancerAssetManager::Get().GetSubclass(Locomotion->AnimInstanceClass);
		OwnerMeshComponent->LinkAnimClassLayers(AnimIns);
		CurrentLinkTag = InTag;
		//广播链接动画层
		if (OnLinkTagChanged.IsBound())
		{
			OnLinkTagChanged.Execute(CurrentLinkTag);
		}
	}
	else
	{
		UE_LOG(CancerLocomotionLog, Warning, TEXT("链接动画层失败 没有找到：%s"), *InTag.ToString())
	}
}

FGameplayTag UCancerLocomotionComponent::GetCurrentAnimLink() const
{
	return CurrentLinkTag;
}
