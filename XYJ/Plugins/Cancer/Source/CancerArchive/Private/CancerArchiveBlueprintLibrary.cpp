
#include "CancerArchiveBlueprintLibrary.h"
#include "CancerArchiveSetting.h"
#include "CancerArchiveSubsystem.h"
#include "CancerArchiveBase.h"
#include "Misc/DateTime.h"
#include "Misc/Crc.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

FString UCancerArchiveBlueprintLibrary::FormatUnixSeconds(int64 UnixSeconds)
{
	const FDateTime DateTime = FDateTime::FromUnixTimestamp(UnixSeconds);
	return DateTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}

FString UCancerArchiveBlueprintLibrary::GetSummarySlotName()
{
	return UCancerArchiveSetting::Get()->SummarySlotName;
}

void UCancerArchiveBlueprintLibrary::SetStructToArchive(const UObject* WorldContextObject, FName Key, const int32& InStruct, bool& bIsValid)
{
}

void UCancerArchiveBlueprintLibrary::GetStructFromArchive(const UObject* WorldContextObject, FName Key, int32& OutStruct, bool& bIsValid)
{
}

bool UCancerArchiveBlueprintLibrary::ContainsStructInArchive(const UObject* WorldContextObject, FName Key)
{
	if (auto* Subsystem = UCancerArchiveSubsystem::Get(WorldContextObject))
	{
		if (auto* Archive = Subsystem->GetArchive<UCancerArchiveBase>(false))
		{
			return Archive->Structs.Contains(Key);
		}
	}
	return false;
}

void UCancerArchiveBlueprintLibrary::RemoveStructFromArchive(const UObject* WorldContextObject, FName Key, bool& bIsValid)
{
	bIsValid = false;
	if (auto* Subsystem = UCancerArchiveSubsystem::Get(WorldContextObject))
	{
		if (auto* Archive = Subsystem->GetArchive<UCancerArchiveBase>(false))
		{
			bIsValid = Archive->Structs.Remove(Key) != 0;
		}
	}
}

TArray<FName> UCancerArchiveBlueprintLibrary::GetArchiveStructKeys(const UObject* WorldContextObject)
{
	TArray<FName> Keys;
	if (auto* Subsystem = UCancerArchiveSubsystem::Get(WorldContextObject))
	{
		if (auto* Archive = Subsystem->GetArchive<UCancerArchiveBase>(false))
		{
			Archive->Structs.GetKeys(Keys);
		}
	}
	return Keys;
}

DEFINE_FUNCTION(UCancerArchiveBlueprintLibrary::execSetStructToArchive)
{
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_PROPERTY(FNameProperty, Key);
	P_GET_OBJECT(UScriptStruct, StructType);
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.Step(Stack.Object, nullptr);
	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructAddr = Stack.MostRecentPropertyAddress;
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.Step(Stack.Object, nullptr);
	FBoolProperty* BoolProp = CastField<FBoolProperty>(Stack.MostRecentProperty);
	void* BoolAddr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	bool bOk = false;
	if (!StructProp || !StructAddr || !BoolProp || !BoolAddr)
	{
		*(bool*)BoolAddr = false;
		return;
	}
	UScriptStruct* ScriptStruct = StructProp->Struct;
	FString Json;
	if (FJsonObjectConverter::UStructToJsonObjectString(ScriptStruct, StructAddr, Json, 0, 0))
	{
		FCancerStructSave Save;
		Save.TypePath = ScriptStruct->GetPathName();
		Save.Json = Json;
		Save.Crc = FCrc::StrCrc32(*Json);
		if (auto* Subsystem = UCancerArchiveSubsystem::Get(WorldContextObject))
		{
			if (auto* Archive = Subsystem->GetArchive<UCancerArchiveBase>(false))
			{
				Archive->Structs.Add(Key, Save);
				bOk = true;
			}
		}
	}
	*(bool*)BoolAddr = bOk;
}

DEFINE_FUNCTION(UCancerArchiveBlueprintLibrary::execGetStructFromArchive)
{
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_PROPERTY(FNameProperty, Key);
	P_GET_OBJECT(UScriptStruct, StructType);
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.Step(Stack.Object, nullptr);
	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructAddr = Stack.MostRecentPropertyAddress;
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.Step(Stack.Object, nullptr);
	FBoolProperty* BoolProp = CastField<FBoolProperty>(Stack.MostRecentProperty);
	void* BoolAddr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	bool bOk = false;
	if (StructType && StructProp && StructAddr && BoolProp && BoolAddr)
	{
		UScriptStruct* ScriptStruct = StructProp->Struct;
		if (ScriptStruct != StructType)
		{
			*(bool*)BoolAddr = false;
			return;
		}
		if (auto* Subsystem = UCancerArchiveSubsystem::Get(WorldContextObject))
		{
			if (auto* Archive = Subsystem->GetArchive<UCancerArchiveBase>(false))
			{
				if (const FCancerStructSave* Found = Archive->Structs.Find(Key))
				{
					if (Found->TypePath == ScriptStruct->GetPathName())
					{
						TSharedPtr<FJsonObject> Obj;
						TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Found->Json);
						if (FJsonSerializer::Deserialize(Reader, Obj) && Obj.IsValid())
						{
							bOk = FJsonObjectConverter::JsonObjectToUStruct(Obj.ToSharedRef(), ScriptStruct, StructAddr, 0, 0);
						}
					}
				}
			}
		}
	}
	*(bool*)BoolAddr = bOk;
}

