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
 /** 어디 대상으로 Taget으로 설정할지 옵션들 (Lyra의 경우, 다양한 옵션 존재) */
UENUM(BlueprintType)
enum class EDlkAbilityTargetingSource : uint8
{
	/** Camera 기준으로 진행 */
	CameraTowardsFocus,
};

UCLASS()
class DEADLOCK_API UDlkGameplayAbility_FromEquipment : public UDlkGameplayAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Dlk|Ability")
	UDlkEquipmentInstance* GetAssociatedEquipment() const;
	UFUNCTION(BlueprintCallable, Category = "Dlk|Ability")
	UDlkInventoryItemInstance* GetAssociatedItem() const;

	int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResults) const;
};
