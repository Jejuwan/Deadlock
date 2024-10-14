// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkWeaponUserInterface.h"
#include "Deadlock/Equipment/DlkEquipmentManagerComponent.h"
#include "Deadlock/Weapons/DlkWeaponInstance.h"

UDlkWeaponUserInterface::UDlkWeaponUserInterface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UDlkWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Pawn을 가져오고
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		// EquipmentManaterComponent를 활용하여, WeaponInstance를 가져오자
		if (UDlkEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UDlkEquipmentManagerComponent>())
		{
			//if (UDlkWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UDlkWeaponInstance>())
			//{
			//	if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
			//	{
			//		// 새로 업데이트해주고, OnWeaponChanged 호출 진행
			//		UDlkWeaponInstance* OldWeapon = CurrentInstance;
			//		CurrentInstance = NewInstance;
			//		OnWeaponChanged(OldWeapon, CurrentInstance);
			//	}
			//}
		}
	}
}