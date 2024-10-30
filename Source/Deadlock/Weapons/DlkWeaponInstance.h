// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Deadlock/Cosmetics/DlkCosmeticAnimationTypes.h"
#include "Deadlock/Equipment/DlkEquipmentInstance.h"
#include "DlkWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkWeaponInstance : public UDlkEquipmentInstance
{
	GENERATED_BODY()
public:
	UDlkWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Weapon�� ������ AnimLayer�� �����Ͽ� ��ȯ */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	/** Weapon�� Equip/Unequip�� ���� Animation Set ������ ��� �ִ� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FDlkAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FDlkAnimLayerSelectionSet UnequippedAnimSet;

protected:
	/** Returns the owning Pawn's Platform User ID */
	UFUNCTION(BlueprintCallable)
	const FPlatformUserId GetOwningUserId() const;

	/** Callback for when the owning pawn of this weapon dies. Removes all spawned device properties. */
	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);

	/** Remove any device proeprties that were activated in ApplyDeviceProperties. */
	void RemoveDeviceProperties();
};
