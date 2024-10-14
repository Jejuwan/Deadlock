// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkWeaponUserInterface.h"
#include "Deadlock/Equipment/DlkEquipmentManagerComponent.h"
#include "Deadlock/Weapons/DlkWeaponInstance.h"

UDlkWeaponUserInterface::UDlkWeaponUserInterface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UDlkWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Pawn�� ��������
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		// EquipmentManaterComponent�� Ȱ���Ͽ�, WeaponInstance�� ��������
		if (UDlkEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UDlkEquipmentManagerComponent>())
		{
			//if (UDlkWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UDlkWeaponInstance>())
			//{
			//	if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
			//	{
			//		// ���� ������Ʈ���ְ�, OnWeaponChanged ȣ�� ����
			//		UDlkWeaponInstance* OldWeapon = CurrentInstance;
			//		CurrentInstance = NewInstance;
			//		OnWeaponChanged(OldWeapon, CurrentInstance);
			//	}
			//}
		}
	}
}