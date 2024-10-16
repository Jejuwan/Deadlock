#pragma once

#include "GameplayTagContainer.h"
#include "DlkInventoryItemDefinition.h"
#include "DlkInventoryFragment_SetStats.generated.h"

/** forward declarations */
class UDlkInventoryItemInstance;

UCLASS()
class UDlkInventoryFragment_SetStats : public UDlkInventoryItemFragment
{
	GENERATED_BODY()

	virtual void OnInstanceCreated(UDlkInventoryItemInstance* Instance) const override;

	/** InitialItemStats gives constructor's parameters for DlkGameplayTagStackContainer */
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;
};