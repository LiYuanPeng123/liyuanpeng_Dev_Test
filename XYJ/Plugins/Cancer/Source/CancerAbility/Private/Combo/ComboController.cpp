#include "Combo/ComboController.h"
#include "Combo/ComboGraph.h"
#include "Combo/MoveDefinition.h"
#include "Engine/World.h"

// 构造：关闭 Tick，仅事件驱动
UComboController::UComboController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboController::BeginPlay()
{
	Super::BeginPlay();
}

void UComboController::InitializeGraph(UComboGraph* InGraph)
{
	ComboGraph = InGraph;
}

void UComboController::StartCombo(UMoveDefinition* StartMove)
{
	if (StartMove)
	{
		CurrentMove = StartMove;
	}
	else if (ComboGraph && !ComboGraph->StartTransitions.Items.IsEmpty())
	{
		FGameplayTagContainer Combined;
		for (const FGameplayTagContainer& Buf : InputBuffer)
		{
			Combined.AppendTags(Buf);
		}
		for (const FComboResolvedInput& i : AttackPreInputs)
		{
			Combined.AppendTags(i.Tags);
		}
		for (const FComboResolvedInput& i : CancelPreInputs)
		{
			Combined.AppendTags(i.Tags);
		}
		UMoveDefinition* BestMove = nullptr;
		int32 BestScore = INT_MIN;
		for (const FTransition& T : ComboGraph->StartTransitions.Items)
		{
			int32 MatchCount = 0;
			TArray<FGameplayTag> RequiredTags;
			T.InputTags.GetGameplayTagArray(RequiredTags);
			for (const FGameplayTag& Tag : RequiredTags)
			{
				if (Combined.HasTag(Tag))
				{
					MatchCount++;
				}
			}
			if (!StateTags.HasAll(T.RequiredStateTags)) continue;
			FGameplayTagContainer DirTags;
			DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Forward")), false));
			DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Back")), false));
			DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Left")), false));
			DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Right")), false));
			const bool bHasDir = Combined.HasAny(DirTags);
			FGameplayTagContainer ModTags;
			ModTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Hold")), false));
			ModTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Double")), false));
			const bool bHasMod = Combined.HasAny(ModTags);
			const int32 Score = MatchCount * 100 + (bHasDir ? 30 : 0) + (bHasMod ? 20 : 0) - (T.Priority * 10);
			if (Score > BestScore)
			{
				BestScore = Score;
				BestMove = T.TargetMove;
			}
		}
		CurrentMove = BestMove;
	}
	else
	{
		CurrentMove = nullptr;
	}
	if (CurrentMove)
	{
		OnMoveStarted.Broadcast(CurrentMove);
	}
}

void UComboController::BufferInput(const FGameplayTagContainer& Input)
{
	if (bComboWindowOpen)
	{
		InputBuffer.Add(Input);
	}
	else if (bPreInputOpen)
	{
		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		FComboResolvedInput Resolved;
		Resolved.Tags = Input;
		Resolved.Timestamp = Now;
		StagePreInput(Resolved);
	}
}

void UComboController::SetStateTags(const FGameplayTagContainer& Tags)
{
	StateTags = Tags;
}

void UComboController::NotifyComboWindowOpen()
{
	bComboWindowOpen = true;
	// 同时开启的优先级：若取消窗口已开且配置为 CancelFirst，则不在此处消费攻击预输入
	const bool bDeferAttackConsume = (bCancelWindowOpen && PreInputConfig && PreInputConfig->SimultaneousPriority == EComboSimultaneousPriority::CancelFirst);
	if (!bDeferAttackConsume)
	{
		FGameplayTagContainer Best = ConsumeBestPreInput(EComboWindowType::Combo);
		if (!Best.IsEmpty())
		{
			InputBuffer.Add(Best);
		}
	}
	// 进行派生尝试
	UMoveDefinition* Next = EvaluateNextMove();
	if (Next)
	{
		OnMoveEnded.Broadcast(CurrentMove);
		CurrentMove = Next;
		OnMoveStarted.Broadcast(CurrentMove);
		ClearBuffer();
	}
}

void UComboController::NotifyComboWindowClose()
{
	bComboWindowOpen = false;
	ClearBuffer();
}

void UComboController::NotifyCancelWindowOpen()
{
	bCancelWindowOpen = true;
	// 同时开启的优先级：若连招窗口已开且配置为 ComboFirst，则不在此处消费取消预输入
	const bool bDeferCancelConsume = (bComboWindowOpen && PreInputConfig && PreInputConfig->SimultaneousPriority == EComboSimultaneousPriority::ComboFirst);
	if (!bDeferCancelConsume)
	{
		FGameplayTagContainer Best = ConsumeBestPreInput(EComboWindowType::Cancel);
		if (!Best.IsEmpty())
		{
			InputBuffer.Add(Best);
		}
	}
}

void UComboController::NotifyCancelWindowClose()
{
	bCancelWindowOpen = false;
}

bool UComboController::IsCancelWindowOpen() const
{
	return bCancelWindowOpen;
}

bool UComboController::TryAdvance()
{
	if (!bComboWindowOpen) return false;
	if (!ComboGraph || !CurrentMove) return false;
	if (UMoveDefinition* Next = EvaluateNextMove())
	{
		OnMoveEnded.Broadcast(CurrentMove);
		CurrentMove = Next;
		OnMoveStarted.Broadcast(CurrentMove);
		ClearBuffer();
		return true;
	}
	return false;
}

UMoveDefinition* UComboController::GetCurrentMove() const
{
	return CurrentMove;
}

UMoveDefinition* UComboController::EvaluateNextMove() const
{
	if (!ComboGraph || !CurrentMove) return nullptr;
	TArray<FTransition> Ts = ComboGraph->GetTransitions(CurrentMove);
	if (Ts.IsEmpty()) return nullptr;

	// 合并输入缓冲为一个标签集合用于匹配评分
	FGameplayTagContainer Combined;
	for (const FGameplayTagContainer& Buf : InputBuffer)
	{
		Combined.AppendTags(Buf);
	}

	UMoveDefinition* Best = nullptr;
	int32 BestScore = INT_MIN;

	for (const FTransition& T : Ts)
	{
		// 计算输入覆盖度：匹配到多少标签
		int32 MatchCount = 0;
		TArray<FGameplayTag> RequiredTags;
		T.InputTags.GetGameplayTagArray(RequiredTags);
		for (const FGameplayTag& Tag : RequiredTags)
		{
			if (Combined.HasTag(Tag))
			{
				MatchCount++;
			}
		}
		if (MatchCount == 0) continue;

		if (!StateTags.HasAll(T.RequiredStateTags)) continue;
		// 方向与修饰加权
		FGameplayTagContainer DirTags;
		DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Forward")), false));
		DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Back")), false));
		DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Left")), false));
		DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Right")), false));
		const bool bHasDir = Combined.HasAny(DirTags);

		FGameplayTagContainer ModTags;
		ModTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Hold")), false));
		ModTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Double")), false));
		const bool bHasMod = Combined.HasAny(ModTags);
		// 评分：覆盖数量*100 + 方向加权 + 修饰加权 - 优先级惩罚
		const int32 Score = MatchCount * 100 + (bHasDir ? 30 : 0) + (bHasMod ? 20 : 0) - (T.Priority * 10);
		if (Score > BestScore)
		{
			BestScore = Score;
			Best = T.TargetMove;
		}
	}
	return Best;
}

void UComboController::ClearBuffer()
{
	InputBuffer.Reset();
}

void UComboController::StagePreInput(const FComboResolvedInput& Input)
{
	// 预输入暂存：按分类分桶（取消/攻击），并应用容量限制
	PurgeOldPreInputs();
	if (IsCancelTag(Input.Tags))
	{
		CancelPreInputs.Add(Input);
		while (PreInputConfig && CancelPreInputs.Num() > PreInputConfig->MaxPreInputCountPerBucket)
		{
			CancelPreInputs.RemoveAt(0);
		}
	}
	else if (IsAttackTag(Input.Tags))
	{
		AttackPreInputs.Add(Input);
		while (PreInputConfig && AttackPreInputs.Num() > PreInputConfig->MaxPreInputCountPerBucket)
		{
			AttackPreInputs.RemoveAt(0);
		}
	}
}

void UComboController::PurgeOldPreInputs()
{
	// 清理超过最大保留时长的预输入，避免旧输入影响新窗口
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float MaxAge = PreInputConfig ? PreInputConfig->MaxPreInputAgeSeconds : 0.4f;
	for (int32 i = CancelPreInputs.Num() - 1; i >= 0; --i)
	{
		if (Now - CancelPreInputs[i].Timestamp > MaxAge)
		{
			CancelPreInputs.RemoveAt(i);
		}
	}
	for (int32 i = AttackPreInputs.Num() - 1; i >= 0; --i)
	{
		if (Now - AttackPreInputs[i].Timestamp > MaxAge)
		{
			AttackPreInputs.RemoveAt(i);
		}
	}
}

FGameplayTagContainer UComboController::ConsumeBestPreInput(EComboWindowType WindowType)
{
	// 在指定窗口类型下，从对应分桶选择评分最高的预输入并消费
	PurgeOldPreInputs();
	TArray<FComboResolvedInput>* Bucket = nullptr;
	if (WindowType == EComboWindowType::Cancel)
	{
		Bucket = &CancelPreInputs;
	}
	else
	{
		Bucket = &AttackPreInputs;
	}
	if (!Bucket || Bucket->Num() == 0)
	{
		return FGameplayTagContainer();
	}
	int32 BestIdx = 0;
	int32 BestScore = INT_MIN;
	for (int32 i = 0; i < Bucket->Num(); ++i)
	{
		const int32 S = ScorePreInput((*Bucket)[i]);
		if (S > BestScore)
		{
			BestScore = S;
			BestIdx = i;
		}
	}
	FGameplayTagContainer Out = (*Bucket)[BestIdx].Tags;
	Bucket->RemoveAt(BestIdx);
	return Out;
}

int32 UComboController::ScorePreInput(const FComboResolvedInput& Input) const
{
	// 预输入评分：时间策略 + 手势权重（和弦/长按/双击）
	int32 Score = 0;
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (PreInputConfig && PreInputConfig->TimePolicy == EComboPreInputTimePolicy::LastPressWins)
	{
		Score += static_cast<int32>((Input.Timestamp) * 1000.f);
	}
	else
	{
		Score -= static_cast<int32>((Input.Timestamp) * 1000.f);
	}
	const int32 ChordW = PreInputConfig ? PreInputConfig->ChordWeight : 3;
	const int32 HoldW = PreInputConfig ? PreInputConfig->HoldWeight : 2;
	const int32 DoubleW = PreInputConfig ? PreInputConfig->DoubleTapWeight : 1;
	// Chord: 同时存在方向标签与输入标签
	FGameplayTagContainer DirTags;
	DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Forward")), false));
	DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Back")), false));
	DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Left")), false));
	DirTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Right")), false));
	const bool bHasDir = Input.Tags.HasAny(DirTags);
	const bool bHasInput = IsAttackTag(Input.Tags) || IsCancelTag(Input.Tags);
	if (bHasDir && bHasInput) Score += ChordW * 100;
	if (Input.Tags.HasTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Hold")), false))) Score += HoldW * 100;
	if (Input.Tags.HasTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Double")), false))) Score += DoubleW * 100;
	return Score;
}

bool UComboController::IsCancelTag(const FGameplayTagContainer& Tags) const
{
	// 取消类：优先使用配置；默认包含格挡/闪避标签
	if (PreInputConfig && !PreInputConfig->CancelInputTags.IsEmpty())
	{
		return Tags.HasAny(PreInputConfig->CancelInputTags);
	}
	// 默认：包含格挡/闪避
	FGameplayTagContainer DefaultCancel;
	DefaultCancel.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tag.Input.Block")), false));
	DefaultCancel.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tag.Input.Evade")), false));
	return Tags.HasAny(DefaultCancel);
}

bool UComboController::IsAttackTag(const FGameplayTagContainer& Tags) const
{
	// 攻击类：优先使用配置；默认包含轻/重攻击标签
	if (PreInputConfig && !PreInputConfig->AttackInputTags.IsEmpty())
	{
		return Tags.HasAny(PreInputConfig->AttackInputTags);
	}
	// 默认：包含轻/重
	FGameplayTagContainer DefaultAttack;
	DefaultAttack.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tag.Input.Light")), false));
	DefaultAttack.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tag.Input.Heavy")), false));
	return Tags.HasAny(DefaultAttack);
}

void UComboController::NotifyPreInputOpen()
{
	bPreInputOpen = true;
}

void UComboController::NotifyPreInputClose()
{
	bPreInputOpen = false;
	PreInputBuffer.Reset();
	AttackPreInputs.Reset();
	CancelPreInputs.Reset();
}

