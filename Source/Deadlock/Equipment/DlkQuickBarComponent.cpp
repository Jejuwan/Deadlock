// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkQuickBarComponent.h"
#include "DlkEquipmentDefinition.h"
#include "DlkEquipmentInstance.h"
#include "DlkEquipmentManagerComponent.h"
#include "DEADLOCK/Inventory/DlkInventoryFragment_EquippableItem.h"
#include "DEADLOCK/Inventory/DlkInventoryItemInstance.h"

UDlkQuickBarComponent::UDlkQuickBarComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UDlkQuickBarComponent::BeginPlay()
{
	// NumSlots�� ���� �̸� Slots�� �Ҵ��Ѵ�
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	// �ݵ�� BeginPlay() �ҷ��ִ� ���� ����� ����!
	Super::BeginPlay();
}

UDlkEquipmentManagerComponent* UDlkQuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<UDlkEquipmentManagerComponent>();
		}
	}

	return nullptr;
}

void UDlkQuickBarComponent::UnequipItemInSlot()
{
	// ����� QuickBar�� Controller�� �پ��ִ� Component������, EquipmentManagerComponent�� Controller�� ����(Possess)�ϰ� �ִ� Pawn�� Component�̴�
	if (UDlkEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		// ���� ������ Item�� �ִٸ�,
		if (EquippedItem)
		{
			// EquipementManager�� ����, Pawn�� ��� ������Ų��
			EquipmentManager->UnequipItem(EquippedItem);

			// �׸��� Controller���� EquipItem�� ���¸� ���� ������ ������Ʈ�Ѵ�
			EquippedItem = nullptr;
		}
	}
}

void UDlkQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	// ���� Ȱ��ȭ�� ActiveSlotIndex�� Ȱ���Ͽ� Ȱ��ȭ�� InventoryItemInstance�� ã�´�
	if (UDlkInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		// Slot Item�� ���� (InventoryItemInstance) InventoryFragment_EquippableItem�� Fragment�� ã�´�:
		// - ã�� ���� �����ߴٸ�, ������ �� ���� Inventory Item���� �ǹ��Ѵ�
		if (const UDlkInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UDlkInventoryFragment_EquippableItem>())
		{
			// EquippableItem���� EquipmentDefinition�� ã�´�:
			// - EquipmentDefinition�� �־��, ������ �� �ִ�
			TSubclassOf<UDlkEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef)
			{
				// �Ʒ��� Unequip�̶� ����ϰ� EquipmentManager�� ���� �����Ѵ�
				if (UDlkEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);

					if (EquippedItem)
					{
						// EquippedItem���� �ռ� ���Ҵ� Instiagor�� Slot�� ������� �ִ´�
						EquippedItem->Instigator = SlotItem;
					}
				}
			}
		}
	}
}

void UDlkQuickBarComponent::AddItemToSlot(int32 SlotIndex, UDlkInventoryItemInstance* Item)
{
	// �ش� ������ ����, Slosts�� Add�� ���� �߰��� �ƴ�, Index�� �ٷ� �ִ´�:
	// - �׷� �̸� Pre-size �ߴٴ� ���ε� �̴� BeginPlay()���� �����Ѵ�
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}

UDlkInventoryItemInstance* UDlkQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UDlkInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			//OnRep_Slots();
		}
	}

	return Result;
}

void UDlkQuickBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		// UnequipItem/EquipItem�� ����, NewIndex�� ���� �Ҵ�� Item�� â�� �� ������Ʈ�� �����Ѵ�
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
	}
}

UDlkInventoryItemInstance* UDlkQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}
