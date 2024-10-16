// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Deadlock/AbilitySystem/Abilities/DlkGameplayAbility.h"
#include "DlkGameplayAbility_FromEquipment.generated.h"

class UDlkEquipmentInstance;
class UDlkInventoryItemInstance;
/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameplayAbility_FromEquipment : public UDlkGameplayAbility
{
	GENERATED_BODY()
public:

	UDlkEquipmentInstance* GetAssociatedEquipment() const;
	UDlkInventoryItemInstance* GetAssociatedItem() const;
};
