// Description:switch the language displayed in the editor
// Author:Jiecool
// Date:2024/5/8
// Email:jiecool@qq.com

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LanguageSwitchSettings.generated.h"

UENUM()
enum class ELangs : uint8
{
	ar UMETA(DisplayName = "العربية"),
	de UMETA(DisplayName = "Deutsch"),
	en UMETA(DisplayName = "English"),
	es UMETA(DisplayName = "español"),
	es_419 UMETA(DisplayName = "español (Latinoamérica)"),
	fr UMETA(DisplayName = "français"),
	it UMETA(DisplayName = "italiano"),
	ja UMETA(DisplayName = "日本語"),
	ko UMETA(DisplayName = "한국어"),
	pl UMETA(DisplayName = "polski"),
	pt_BR UMETA(DisplayName = "português (Brasil)"),
	ru UMETA(DisplayName = "русский"),
	tr UMETA(DisplayName = "Türkçe"),
	zh_Hans UMETA(DisplayName = "中文（简体）")
};


UCLASS(config = EditorSettings)
class LANGUAGESWITCH_API ULanguageSwitchSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULanguageSwitchSettings();

	//~ Begin UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;
	virtual FName GetSectionName() const override;
	//~ End UDeveloperSettings interface

public:

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (DisplayName = "SourceLanguage"))
	ELangs SourceLanguage = ELangs::ar;

	UPROPERTY(config, EditAnywhere, Category = Settings, meta = (DisplayName = "TargetLanguage"))
	ELangs TargetLanguage = ELangs::en;

};