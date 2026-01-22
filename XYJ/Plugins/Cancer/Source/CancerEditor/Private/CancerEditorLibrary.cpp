#include "CancerEditorLibrary.h"

#if WITH_EDITOR
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/UnrealType.h"
#include "GameplayTagContainer.h"
#include "UObject/Script.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsSettings.h"
#include "Misc/MessageDialog.h"
#include "Engine/DataTable.h"
#include "FileHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SoftObjectPath.h"
#include "Engine/Blueprint.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "GameplayTagsEditorModule.h"

static bool ReplaceTagInProperty(FProperty* Property, void* ContainerPtr, FName OldTagName, FName NewTagName);

static bool EnsureGameplayTagRegistered(const FName& TagName)
{
	if (UGameplayTagsManager::Get().RequestGameplayTag(TagName, false).IsValid())
	{
		return true;
	}

	IGameplayTagsEditorModule& TagsEditor = IGameplayTagsEditorModule::Get();
	if (TagsEditor.AddNewGameplayTagToINI(TagName.ToString(), TEXT(""), NAME_None, false))
	{
		// Tag added successfully, it should be available now
		return UGameplayTagsManager::Get().RequestGameplayTag(TagName, false).IsValid();
	}

	return false;
}

static bool ReplaceTagInStruct(UScriptStruct* ScriptStruct, void* StructPtr, FName OldTagName, FName NewTagName)
{
	bool bChanged = false;
	for (TFieldIterator<FProperty> It(ScriptStruct); It; ++It)
	{
		bChanged |= ReplaceTagInProperty(*It, StructPtr, OldTagName, NewTagName);
	}
	return bChanged;
}

static bool ReplaceTagsInClassDefaultObject(UClass* Class, FName OldTagName, FName NewTagName, TArray<UObject*>& OutModifiedAssets)
{
	if (!Class)
	{
		return false;
	}
	UObject* CDO = Class->GetDefaultObject();
	if (!CDO)
	{
		return false;
	}
	bool bModified = false;
	for (TFieldIterator<FProperty> It(Class); It; ++It)
	{
		bModified |= ReplaceTagInProperty(*It, CDO, OldTagName, NewTagName);
	}
	if (bModified)
	{
		CDO->Modify();
		CDO->PostEditChange();
		if (UBlueprint* BP = Cast<UBlueprint>(Class->ClassGeneratedBy))
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
			FKismetEditorUtilities::CompileBlueprint(BP);
			OutModifiedAssets.Add(BP);
		}
		else
		{
			OutModifiedAssets.Add(Class);
		}
	}
	return bModified;
}

static void OpenAssetsInEditor(const TArray<UObject*>& Assets)
{
	if (!GEditor)
	{
		return;
	}
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!AssetEditorSubsystem)
	{
		return;
	}
	TArray<UObject*> UniqueAssets;
	for (UObject* Obj : Assets)
	{
		if (Obj && !UniqueAssets.Contains(Obj))
		{
			UniqueAssets.Add(Obj);
		}
	}
	if (UniqueAssets.Num() > 0)
	{
		AssetEditorSubsystem->OpenEditorForAssets(UniqueAssets);
	}
}

static void ShowRestartNotification()
{
#if WITH_EDITOR
	FNotificationInfo Info(NSLOCTEXT("CancerEditor", "RestartNeeded", "标签更新已应用。建议重启编辑器以完全生效。"));
	Info.bFireAndForget = false;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = 0.0f;
	Info.bUseThrobber = false;
	Info.bUseSuccessFailIcons = false;
	Info.bUseLargeFont = false;

	Info.ButtonDetails.Add(FNotificationButtonInfo(
		NSLOCTEXT("CancerEditor", "RestartNow", "重启"),
		NSLOCTEXT("CancerEditor", "RestartNowTT", "重启编辑器以使更改完全生效"),
		FSimpleDelegate::CreateLambda([]()
		{
			FUnrealEdMisc::Get().RestartEditor(false);
		}),
		SNotificationItem::CS_None
	));
	Info.ButtonDetails.Add(FNotificationButtonInfo(
		NSLOCTEXT("CancerEditor", "RestartLater", "稍后"),
		NSLOCTEXT("CancerEditor", "RestartLaterTT", "继续当前操作，不重启"),
		FSimpleDelegate::CreateLambda([](){}),
		SNotificationItem::CS_None
	));

	TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
	if (Notification.IsValid())
	{
		Notification->SetCompletionState(SNotificationItem::CS_Pending);
	}
#endif
}
static bool ReplaceTagInProperty(FProperty* Property, void* ContainerPtr, FName OldTagName, FName NewTagName)
{
	bool bChanged = false;

	// 获取属性值的指针
	void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ContainerPtr);

	if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		if (StructProp->Struct->GetFName() == FName("GameplayTag"))
		{
			FGameplayTag* TagVal = (FGameplayTag*)ValuePtr;
			if (TagVal->GetTagName() == OldTagName)
			{
				// 强制更新 Tag
				*TagVal = UGameplayTagsManager::Get().RequestGameplayTag(NewTagName, true);
				bChanged = true;
			}
		}
		else if (StructProp->Struct->GetFName() == FName("GameplayTagContainer"))
		{
			FGameplayTagContainer* ContainerVal = (FGameplayTagContainer*)ValuePtr;
			// 构造新容器，按名称替换旧标签，避免 OldTag 无效导致 RemoveTag 失败
			TArray<FGameplayTag> ExistingTags;
			for (auto It = ContainerVal->CreateConstIterator(); It; ++It)
			{
				ExistingTags.Add(*It);
			}
			FGameplayTag NewTagResolved = UGameplayTagsManager::Get().RequestGameplayTag(NewTagName, true);
			TArray<FGameplayTag> FinalTags;
			bool bLocalChanged = false;
			for (const FGameplayTag& T : ExistingTags)
			{
				if (T.GetTagName() == OldTagName)
				{
					FinalTags.Add(NewTagResolved);
					bLocalChanged = true;
				}
				else
				{
					FinalTags.Add(T);
				}
			}
			if (bLocalChanged)
			{
				for (const FGameplayTag& T : ExistingTags)
				{
					ContainerVal->RemoveTag(T);
				}
				for (const FGameplayTag& T : FinalTags)
				{
					ContainerVal->AddTag(T);
				}
				bChanged = true;
			}
		}
		else if (StructProp->Struct->GetFName() == FName("GameplayTagQuery"))
		{
			bChanged |= ReplaceTagInStruct(StructProp->Struct, ValuePtr, OldTagName, NewTagName);
		}
		else
		{
			bChanged |= ReplaceTagInStruct(StructProp->Struct, ValuePtr, OldTagName, NewTagName);
		}
	}
	else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper Helper(ArrayProp, ValuePtr);
		for (int32 i = 0; i < Helper.Num(); ++i)
		{
			bChanged |= ReplaceTagInProperty(ArrayProp->Inner, Helper.GetRawPtr(i), OldTagName, NewTagName);
		}
	}
		else if (FMapProperty* MapProp = CastField<FMapProperty>(Property))
	{
		FScriptMapHelper MapHelper(MapProp, ValuePtr);
		// 递归处理值中的标签（不在迭代时直接修改键，避免哈希/索引失效）
		for (int32 Index = 0; Index < MapHelper.Num(); ++Index)
		{
			if (!MapHelper.IsValidIndex(Index)) continue;
			void* ValPtr = MapHelper.GetValuePtr(Index);
			bChanged |= ReplaceTagInProperty(MapProp->ValueProp, ValPtr, OldTagName, NewTagName);
		}

		// 如果键是 GameplayTag，且等于旧标签，则替换键
		if (FStructProperty* KeyStructProp = CastField<FStructProperty>(MapProp->KeyProp))
		{
			if (KeyStructProp->Struct->GetFName() == FName("GameplayTag"))
			{
				for (int32 Index = MapHelper.Num() - 1; Index >= 0; --Index)
				{
					if (!MapHelper.IsValidIndex(Index)) continue;
					FGameplayTag* KeyTag = (FGameplayTag*)MapHelper.GetKeyPtr(Index);
					if (KeyTag && KeyTag->GetTagName() == OldTagName)
					{
						void* OldValPtr = MapHelper.GetValuePtr(Index);
						const int32 NewIdx = MapHelper.AddDefaultValue_Invalid_NeedsRehash();
						FGameplayTag* NewKeyTag = (FGameplayTag*)MapHelper.GetKeyPtr(NewIdx);
						*NewKeyTag = UGameplayTagsManager::Get().RequestGameplayTag(NewTagName, true);
						void* NewValPtr = MapHelper.GetValuePtr(NewIdx);
						MapProp->ValueProp->CopyCompleteValue(NewValPtr, OldValPtr);
						MapHelper.RemoveAt(Index);
						bChanged = true;
					}
				}
				MapHelper.Rehash();
			}
		}
	}
		else if (FSetProperty* SetProp = CastField<FSetProperty>(Property))
	{
		FScriptSetHelper SetHelper(SetProp, ValuePtr);
		if (FStructProperty* ElemStructProp = CastField<FStructProperty>(SetProp->ElementProp))
		{
			if (ElemStructProp->Struct->GetFName() == FName("GameplayTag"))
			{
				for (int32 Index = SetHelper.Num() - 1; Index >= 0; --Index)
				{
					if (!SetHelper.IsValidIndex(Index)) continue;
					FGameplayTag* ElemTag = (FGameplayTag*)SetHelper.GetElementPtr(Index);
					if (ElemTag && ElemTag->GetTagName() == OldTagName)
					{
						const int32 NewIdx = SetHelper.AddDefaultValue_Invalid_NeedsRehash();
						FGameplayTag* NewElemTag = (FGameplayTag*)SetHelper.GetElementPtr(NewIdx);
						*NewElemTag = UGameplayTagsManager::Get().RequestGameplayTag(NewTagName, true);
						SetHelper.RemoveAt(Index);
						bChanged = true;
					}
				}
				SetHelper.Rehash();
			}
			else
			{
				for (int32 Index = 0; Index < SetHelper.Num(); ++Index)
				{
					if (!SetHelper.IsValidIndex(Index)) continue;
					void* ElemPtr = SetHelper.GetElementPtr(Index);
					bChanged |= ReplaceTagInProperty(SetProp->ElementProp, ElemPtr, OldTagName, NewTagName);
				}
			}
		}
		else
		{
			for (int32 Index = 0; Index < SetHelper.Num(); ++Index)
			{
				if (!SetHelper.IsValidIndex(Index)) continue;
				void* ElemPtr = SetHelper.GetElementPtr(Index);
				bChanged |= ReplaceTagInProperty(SetProp->ElementProp, ElemPtr, OldTagName, NewTagName);
			}
		}
	}
	else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
	{
		if (ObjProp->HasAnyPropertyFlags(CPF_InstancedReference | CPF_ContainsInstancedReference))
		{
			UObject* Obj = ObjProp->GetObjectPropertyValue(ValuePtr);
			if (Obj)
			{
				for (TFieldIterator<FProperty> It(Obj->GetClass()); It; ++It)
				{
					bChanged |= ReplaceTagInProperty(*It, Obj, OldTagName, NewTagName);
				}
			}
		}
	}

	return bChanged;
}
#endif


void UCancerEditorLibrary::Edit_RedirectTag(const TArray<FCancerGameplayTagRedirect>& Redirects)
{
#if WITH_EDITOR

	if (Redirects.IsEmpty())
	{
		return;
	}

	TArray<FCancerGameplayTagRedirect> ValidRedirects;
	// Validate redirects
	for (const auto& Redirect : Redirects)
	{
		FName OldTagName = Redirect.OldName;
		FName NewTagName = Redirect.NewName;
		if (OldTagName.IsNone() || NewTagName.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("RedirectTag: Skipping invalid redirect (Old or New tag is None)."));
			continue;
		}

		FGameplayTag OldTag = UGameplayTagsManager::Get().RequestGameplayTag(OldTagName, false);
		FGameplayTag NewTag = UGameplayTagsManager::Get().RequestGameplayTag(NewTagName, false);
		// Note: Old tag might not exist if it was deleted, but we should still allow replacing references to it if we can resolve it by name.
		// However, RequestGameplayTag with ErrorIfNotFound=false returns invalid tag if not found.
		// Let's assume user wants to redirect existing tags. If OldTag is not found in manager, we might still want to search for it in assets by name.
		
		if (!NewTag.IsValid())
		{
			if (!EnsureGameplayTagRegistered(NewTagName))
			{
				UE_LOG(LogTemp, Warning, TEXT("RedirectTag: Failed to auto-register new tag %s"), *NewTagName.ToString());
				continue;
			}
		}
		ValidRedirects.Add(Redirect);
	}

	if (ValidRedirects.IsEmpty())
	{
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TSet<FName> ProcessedPackages;
	bool bAnyAssetModified = false;
	bool bAnyTagDeleted = false;
	TArray<UObject*> ModifiedAssets;

	// Iterate through all valid redirects
	for (const auto& Redirect : ValidRedirects)
	{
		FName OldTagName = Redirect.OldName;
		FName NewTagName = Redirect.NewName;
		// Find assets referencing the old tag
		TArray<FAssetDependency> LinksToAsset;
		FAssetIdentifier AssetId(FGameplayTag::StaticStruct(), OldTagName);
		AssetRegistry.GetReferencers(AssetId, LinksToAsset, UE::AssetRegistry::EDependencyCategory::SearchableName, UE::AssetRegistry::EDependencyQuery::NoRequirements);

		UE_LOG(LogTemp, Log, TEXT("[RedirectTag] Found %d assets referencing tag %s"), LinksToAsset.Num(), *OldTagName.ToString());

		for (const FAssetDependency& Link : LinksToAsset)
		{
			if (ProcessedPackages.Contains(Link.AssetId.PackageName))
			{
				continue;
			}
			
			// Get assets in package
			TArray<FAssetData> AssetsInPackage;
			AssetRegistry.GetAssetsByPackageName(Link.AssetId.PackageName, AssetsInPackage);

			for (const FAssetData& AssetData : AssetsInPackage)
			{
				UObject* Asset = AssetData.GetAsset();
				if (!Asset) continue;

				bool bModified = false;
				
				// Apply ALL redirects to this asset to avoid multiple saves
				for (const auto& InnerRedirect : ValidRedirects)
				{
					FName InnerOldTagName = InnerRedirect.OldName;
					FName InnerNewTagName = InnerRedirect.NewName;

					for (TFieldIterator<FProperty> It(Asset->GetClass()); It; ++It)
					{
						bModified |= ReplaceTagInProperty(*It, Asset, InnerOldTagName, InnerNewTagName);
					}

					// Special handling for Blueprint CDO
					if (UBlueprint* BP = Cast<UBlueprint>(Asset))
					{
						if (UClass* GeneratedClass = BP->GeneratedClass)
						{
							UObject* CDO = GeneratedClass->GetDefaultObject();
							for (TFieldIterator<FProperty> It(GeneratedClass); It; ++It)
							{
								bModified |= ReplaceTagInProperty(*It, CDO, InnerOldTagName, InnerNewTagName);
							}
						}
					}
					else if (UDataTable* DataTable = Cast<UDataTable>(Asset))
					{
						if (UScriptStruct* RowStruct = DataTable->RowStruct)
						{
							for (auto& RowIt : DataTable->GetRowMap())
							{
								uint8* RowData = RowIt.Value;
								bModified |= ReplaceTagInStruct(RowStruct, RowData, InnerOldTagName, InnerNewTagName);

								for (TFieldIterator<FProperty> RowPropIt(RowStruct); RowPropIt; ++RowPropIt)
								{
									FProperty* RowProp = *RowPropIt;
									void* RowValPtr = RowProp->ContainerPtrToValuePtr<void>(RowData);

									if (FClassProperty* ClassProp = CastField<FClassProperty>(RowProp))
									{
										UClass* AbilityClass = Cast<UClass>(ClassProp->GetObjectPropertyValue(RowValPtr));
										if (AbilityClass)
										{
											bModified |= ReplaceTagsInClassDefaultObject(AbilityClass, InnerOldTagName, InnerNewTagName, ModifiedAssets);
										}
									}
									else if (FObjectProperty* ObjProp2 = CastField<FObjectProperty>(RowProp))
									{
										UObject* Obj = ObjProp2->GetObjectPropertyValue(RowValPtr);
										if (Obj)
										{
											UClass* TargetClass = Obj->GetClass();
											bModified |= ReplaceTagsInClassDefaultObject(TargetClass, InnerOldTagName, InnerNewTagName, ModifiedAssets);
										}
									}
									else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(RowProp))
									{
										FSoftObjectPath* PathPtr = reinterpret_cast<FSoftObjectPath*>(RowValPtr);
										if (PathPtr && PathPtr->IsValid())
										{
											UObject* LoadedObj = PathPtr->TryLoad();
											if (LoadedObj)
											{
												UClass* TargetClass = Cast<UClass>(LoadedObj) ? Cast<UClass>(LoadedObj) : LoadedObj->GetClass();
												bModified |= ReplaceTagsInClassDefaultObject(TargetClass, InnerOldTagName, InnerNewTagName, ModifiedAssets);
											}
										}
									}
									else if (FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(RowProp))
									{
										FSoftObjectPath* PathPtr = reinterpret_cast<FSoftObjectPath*>(RowValPtr);
										if (PathPtr && PathPtr->IsValid())
										{
											UClass* LoadedClass = Cast<UClass>(PathPtr->TryLoad());
											if (LoadedClass)
											{
												bModified |= ReplaceTagsInClassDefaultObject(LoadedClass, InnerOldTagName, InnerNewTagName, ModifiedAssets);
											}
										}
									}
								}
							}
						}
					}
				}


				

				if (bModified)
				{
					Asset->Modify();
					Asset->MarkPackageDirty();
					Asset->PostEditChange();
					if (UBlueprint* BPModified = Cast<UBlueprint>(Asset))
					{
						FBlueprintEditorUtils::MarkBlueprintAsModified(BPModified);
						FKismetEditorUtilities::CompileBlueprint(BPModified);
					}
					UE_LOG(LogTemp, Log, TEXT("[RedirectTag] Updated Asset: %s"), *Asset->GetName());
					bAnyAssetModified = true;
				}
			}
			ProcessedPackages.Add(Link.AssetId.PackageName);
		}
	}

	// 移除全量回扫以避免性能问题和潜在递归崩溃

	// Handle tag deletion
	for (const auto& Redirect : ValidRedirects)
	{
		if (Redirect.bDeleteOldTag)
		{
			RemoveTag(FGameplayTag::RequestGameplayTag(Redirect.OldName,false));
			bAnyTagDeleted = true;
		}
	}
	if (bAnyTagDeleted)
	{
		ShowRestartNotification();
	}

	if (bAnyAssetModified)
	{
		FEditorFileUtils::SaveDirtyPackages(true, true, true, false, false, false);
		OpenAssetsInEditor(ModifiedAssets);
	}
	
#endif
}

void UCancerEditorLibrary::RemoveTag(FGameplayTag InTag)
{
	FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultGameplayTags.ini");
	FString SectionName = TEXT("/Script/GameplayTags.GameplayTagsSettings");
	GConfig->LoadFile(ConfigPath);
	FConfigFile* ConfigFile = GConfig->FindConfigFile(ConfigPath);
	FConfigSection* Section =const_cast<FConfigSection*>(ConfigFile->FindOrAddConfigSection(SectionName));
	
	FName KeyName(TEXT("+GameplayTagList"));
	TArray<FConfigValue> Values;
	Section->MultiFind(KeyName, Values); // 获取当前所有标签配置
	Section->Remove(KeyName); 
	
	for (const FConfigValue& Val : Values)
	{
		if (!Val.GetValue().Contains(FString::Printf(TEXT("Tag=\"%s\""), *InTag.ToString())))
		{
			Section->Add(KeyName, Val);
		}
	}
	ConfigFile->Dirty = true;
	ConfigFile->Write(ConfigPath);
	// We only request restart if we actually modified the config. 
	// The calling function will handle restart if needed, or we can do it here.
	// Since RemoveTag is static and might be called in loop, we should be careful.
	// However, RequestRestartEditor shows a dialog, which is annoying in a loop.
	// We should probably move restart request out of the loop.
}


void UCancerEditorLibrary::RequestRestartEditor()
{
#if WITH_EDITOR
		const EAppReturnType::Type ReturnValue = FMessageDialog::Open(
			EAppMsgType::YesNo,
			NSLOCTEXT("Cancer", "RestartEditorMsg", "Are you sure you want to restart the editor?")
		);
		if (ReturnValue == EAppReturnType::No)
		{
			return;
		}
	FUnrealEdMisc::Get().RestartEditor(false);
#endif
}
