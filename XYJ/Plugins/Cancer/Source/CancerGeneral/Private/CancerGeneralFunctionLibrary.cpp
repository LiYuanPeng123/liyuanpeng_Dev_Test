#include "CancerGeneralFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"


EWindowMode::Type UCancerGeneralFunctionLibrary::GetEnumFromString(const FString & Value)
{

	return ENUMUtility::GetEnumValueFromString<EWindowMode::Type>("EWindowMode::Type",Value);
}

void UCancerGeneralFunctionLibrary::SetRHI(const ERHI& Value)
{
	FString CurrentPlatform = UGameplayStatics::GetPlatformName();
	FString DefaultGraphicsRHI;
	if (CurrentPlatform == "windows")
	{
		GConfig->GetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSetting"),
		                   TEXT("DefaultGraphicsRHI"), DefaultGraphicsRHI, GEngineIni);

		switch (Value)
		{
		case ERHI::DX11:
			if (DefaultGraphicsRHI != "DefaultGraphicsRHI_DX11")
			{
				GConfig->SetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSetting"),
				                   TEXT("DefaultGraphicsRHI"),TEXT("DefaultGraphicsRHI_DX11"), GEngineIni);
				GConfig->Flush(true, GEngineIni);
			}
			break;
		case ERHI::DX12:
			if (DefaultGraphicsRHI != "DefaultGraphicsRHI_DX12")
			{
				GConfig->SetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSetting"),
				                   TEXT("DefaultGraphicsRHI"),TEXT("DefaultGraphicsRHI_DX12"), GEngineIni);
				GConfig->Flush(true, GEngineIni);
			}
			break;
		case ERHI::VULKAN:
			if (DefaultGraphicsRHI != "DefaultGraphicsRHI_Vulkan")
			{
				GConfig->SetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSetting"),
				                   TEXT("DefaultGraphicsRHI"),TEXT("DefaultGraphicsRHI_Vulkan"), GEngineIni);
				GConfig->Flush(true, GEngineIni);
			}
			break;
		}
	}
}

ERHI UCancerGeneralFunctionLibrary::GetRHI()
{
	FString CurrentPlatform = UGameplayStatics::GetPlatformName();
	FString DefaultGraphicsRHI;
	GConfig->GetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSetting"),
	                   TEXT("DefaultGraphicsRHI"), DefaultGraphicsRHI, GEngineIni);
	if (DefaultGraphicsRHI == "DefaultGraphicsRHI_DX11")
	{
		return ERHI::DX11;
	}
	else if (DefaultGraphicsRHI == "DefaultGraphicsRHI_DX12")
	{
		return ERHI::DX12;
	}
	return ERHI::VULKAN;
}

