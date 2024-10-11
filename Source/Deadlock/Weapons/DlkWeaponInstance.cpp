// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkWeaponInstance.h"

UDlkWeaponInstance::UDlkWeaponInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

TSubclassOf<UAnimInstance> UDlkWeaponInstance::PickBestAnimLayer(bool bEquipped,
	const FGameplayTagContainer& CosmeticTags) const
{
	const FDlkAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnequippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}

