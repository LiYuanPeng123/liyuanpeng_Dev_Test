#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CheatManager.h"
#include "CancerInventoryItemCheats.generated.h"

class UCancerInventoryComponent;
/**
 * 
 */
UCLASS(NotBlueprintable)
class CANCERINVENTORY_API UCancerInventoryItemCheats : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UCancerInventoryItemCheats();

	UFUNCTION(Exec, BlueprintCallable)
	void AddItem(int32 ItemID, int32 Count);

	UFUNCTION(Exec, BlueprintCallable)
	void RemoveItem(int32 ItemID, int32 Count);

	// 清除快捷槽
	UFUNCTION(Exec, BlueprintCallable)
	void ClearQuickBarItem();
	
	// 将武器添加到快捷槽
	UFUNCTION(Exec, BlueprintCallable)
	void MoveWeaponItemToQuickBar();
private:
	UCancerInventoryComponent* GetInventoryComponent() const;
};
