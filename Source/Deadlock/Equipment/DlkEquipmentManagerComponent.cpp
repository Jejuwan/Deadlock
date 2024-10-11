// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkEquipmentManagerComponent.h"
#include "DlkEquipmentDefinition.h"
#include "DlkEquipmentInstance.h"

UDlkEquipmentInstance* FDlkEquipmentList::AddEntry(TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition)
{
	UDlkEquipmentInstance* Result = nullptr;
	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	// EquipmentDefinition�� ��� �������� EditDefaultsOnly�� ���ǵǾ� �־� GetDefault�� ��� �͵� �츮���� �ʿ��� �͵��� ��� ����ִ�
	const UDlkEquipmentDefinition* EquipmentCDO = GetDefault<UDlkEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UDlkEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UDlkEquipmentInstance::StaticClass();
	}

	// Entries�� �߰�������
	FDlkAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UDlkEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	// ActorsToSpawn�� ����, Actor���� �ν��Ͻ�ȭ ������
	// - ���? EquipmentInstance��!
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	return Result;
}

void FDlkEquipmentList::RemoveEntry(UDlkEquipmentInstance* Instance)
{
	// �ܼ��� �׳� Entries�� ��ȸ�ϸ�, Instance�� ã�Ƽ�
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FDlkAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			// Actor ���� �۾� �� iterator�� ���� �����ϰ� Array���� ���� ����
			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

UDlkEquipmentManagerComponent::UDlkEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
                                                                                                            , EquipmentList(this)
{
}

UDlkEquipmentInstance* UDlkEquipmentManagerComponent::EquipItem(TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition)
{
	UDlkEquipmentInstance* Result = nullptr;
	if (EquipmentDefinition)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void UDlkEquipmentManagerComponent::UnequipItem(UDlkEquipmentInstance* ItemInstance)
{
	if (ItemInstance)
	{
		// �ش� �Լ��� BP�� Event��带 ȣ�����ش� (�ڼ��Ѱ� �ش� �Լ� �����ϸ鼭 ����)
		ItemInstance->OnUnequipped();

		// EquipementList�� �������ش�:
		// - �����ϴ� ������ ���� �߰��Ǿ��� Actor Instance�� ���Ÿ� �����Ѵ�
		EquipmentList.RemoveEntry(ItemInstance);
	}
}
