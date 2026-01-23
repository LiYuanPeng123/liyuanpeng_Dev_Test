#include "XYJ_Dummy.h"

#include "CancerAssetManager.h"
#include "Character/PawnData.h"

AXYJ_Dummy::AXYJ_Dummy()
{
}

void AXYJ_Dummy::BeginPlay()
{
	Super::BeginPlay();
	//加载组件
	auto ActionData = UCancerAssetManager::Get().GetAsset(PawnData->ActionData);
	TSharedPtr<FStreamableHandle> Handle = ActionData->StartAsyncLoadAndExecuteWithHandle(
		this, FSimpleDelegate::CreateLambda(
			[this, ActionData]()
			{
				ActionData->PreInitComponent();
				ActionData->PostInitComponent();
				ActionData->FinalizeAfterComponent();
			}));
}
