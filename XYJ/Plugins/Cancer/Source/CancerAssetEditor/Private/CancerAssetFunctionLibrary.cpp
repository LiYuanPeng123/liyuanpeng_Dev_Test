
#include"CancerAssetFunctionLibrary.h"

#include "EditorUtilitySubsystem.h"

bool UCancerAssetFunctionLibrary::IsFirstCharDigit(const FString& InString)
{
	if (InString.Len() > 0 && TChar<TCHAR>::IsDigit(InString[0]))
	{
		return true;
	}
	return false;
}
