#pragma once

#include "DlkInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "DlkInventoryFragment_EquippableItem.generated.h"

/** forward declaration */
class UDlkEquipmentDefinition;

UCLASS()
class UDlkInventoryFragment_EquippableItem : public UDlkInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Dlk)
	TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition;
};