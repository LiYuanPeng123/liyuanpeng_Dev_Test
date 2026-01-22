

#include "AssetActionUtility/Animation/AnimationAssetAction.h"

#include "AnimationBlueprintLibrary.h"
#include "AnimationModifier.h"
#include "AssetExportTask.h"
#include "ContentBrowserModule.h"
#include "EditorUtilityLibrary.h"
#include "IContentBrowserSingleton.h"
#include "UnrealExporter.h"
#include "Exporters/Exporter.h"

void UAnimationAssetAction::EditAnimation_RemoveCurvesAndNotifies()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(
		"ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	
	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{
			
					UAnimationBlueprintLibrary::RemoveAllCurveData(AnimSequence);
					UAnimationBlueprintLibrary::RemoveAllAnimationNotifyTracks(AnimSequence);
					
					AnimSequence->MarkPackageDirty();
				}
			}
		}
	}
}

void UAnimationAssetAction::EditAnimation_Curves()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(
		"ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	
	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{
			
					UAnimationBlueprintLibrary::RemoveAllCurveData(AnimSequence);
					
					AnimSequence->MarkPackageDirty();
				}
			}
		}
	}
}

void UAnimationAssetAction::EditAnimation_Notifies()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(
		"ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	
	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{
					
					UAnimationBlueprintLibrary::RemoveAllAnimationNotifyTracks(AnimSequence);
					
					AnimSequence->MarkPackageDirty();
				}
			}
		}
	}
}

void UAnimationAssetAction::EditAnimation_AddNotifies(TMap<FName, TSubclassOf<UAnimNotify>> NotifyMaps)
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{
					for (auto it = NotifyMaps.CreateConstIterator(); it; ++it)
					{
						//添加动画通知轨道
						UAnimationBlueprintLibrary::AddAnimationNotifyTrack(AnimSequence,it.Key());
						//添加动画通知
						UAnimationBlueprintLibrary::AddAnimationNotifyEvent(
							AnimSequence,it.Key(),0.0f,it.Value());
					}
					AnimSequence->MarkPackageDirty();
				}
			}
		} 
	}
}

void UAnimationAssetAction::EditAnimation_AddNotifyState(TMap<FName, TSubclassOf<UAnimNotifyState>> NotifyStateMaps)
{

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{
					for (auto it = NotifyStateMaps.CreateConstIterator(); it; ++it)
					{
						//添加动画通知轨道
						UAnimationBlueprintLibrary::AddAnimationNotifyTrack(AnimSequence,it.Key());
						//添加动画通知状态
						UAnimationBlueprintLibrary::AddAnimationNotifyStateEvent(
							AnimSequence,it.Key(),0.0f,2.0f,it.Value());
					}
					AnimSequence->MarkPackageDirty();
				}
			}
		} 
	}
}

void UAnimationAssetAction::EditAnimation_AddModifier(TArray<TSubclassOf<UAnimationModifier>> Modifier)
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{
					for (auto MClass:Modifier)
					{
						if (UAnimationModifier * NewModifier =NewObject<UAnimationModifier>(this,MClass))
						{
							NewModifier->RevertFromAnimationSequence(AnimSequence);
							NewModifier->ApplyToAnimationSequence(AnimSequence);
						}
					}
					AnimSequence->MarkPackageDirty();
				}
			}
		} 
	}
}

void UAnimationAssetAction::EditAnimation_SetCurveSetting(UAnimCurveCompressionSettings* Obj)
{
	
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	for (auto AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (UObject* Asset = AssetData.GetAsset())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
				{

					AnimSequence->CurveCompressionSettings = Obj;
					AnimSequence->MarkPackageDirty();
				}
			}
		} 
	}
}

void UAnimationAssetAction::ExportSelectedToFBX()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	// 导出目录
	FString ExportFolder = FPaths::ProjectDir() / TEXT("ExportedFBX/");
	IFileManager::Get().MakeDirectory(*ExportFolder, true);

	for (UObject* Asset : SelectedAssets)
	{
		FString FileName;
		UObject* ExportObject = nullptr;

		if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Asset))
		{
			FileName = ExportFolder / (SkeletalMesh->GetName() + TEXT(".fbx"));
			ExportObject = SkeletalMesh;
		}
		else if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset))
		{
			FileName = ExportFolder / (AnimSequence->GetName() + TEXT(".fbx"));
			ExportObject = AnimSequence;
		}
		else if (USkeleton* Skeleton = Cast<USkeleton>(Asset))
		{
			FileName = ExportFolder / (Skeleton->GetName() + TEXT(".fbx"));
			ExportObject = Skeleton;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("不支持导出该类型资产: %s"), *Asset->GetName());
			continue;
		}

		// 构建导出任务
		if (ExportObject)
		{
			UAssetExportTask* ExportTask = NewObject<UAssetExportTask>();
			ExportTask->Object = ExportObject;
			ExportTask->Filename = FileName;
			ExportTask->bAutomated = true;
			ExportTask->bReplaceIdentical = true;
			ExportTask->bPrompt = false;
			ExportTask->bUseFileArchive = false;

			// 执行导出
			UExporter::RunAssetExportTask(ExportTask);

			UE_LOG(LogTemp, Warning, TEXT("已导出 [%s] -> %s"), *ExportObject->GetName(), *FileName);
		}
	}
}
