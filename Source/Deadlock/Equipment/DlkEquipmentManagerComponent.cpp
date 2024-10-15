// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkEquipmentManagerComponent.h"
#include "DlkEquipmentDefinition.h"
#include "DlkEquipmentInstance.h"
#include "AbilitySystemGlobals.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"

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

	UDlkAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	{
		for (const TObjectPtr<UDlkAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
		}
	}

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
			UDlkAbilitySystemComponent* ASC = GetAbilitySystemComponent();
			check(ASC);
			{
				// TakeFromAbilitySystem�� GiveToAbilitySystem �ݴ� ��Ȱ��, ActivatableAbilities���� �����Ѵ�
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			// Actor ���� �۾� �� iterator�� ���� �����ϰ� Array���� ���� ����
			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

UDlkAbilitySystemComponent* FDlkEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();

	// GetAbilitySystemComponentFromActor�� ��� Ȯ���غ���:
	// - EquipmentManagerComponent�� ADlkCharacter�� Owner�� ������ �ִ�
	// - �ش� �Լ��� IAbilitySystemInterface�� ���� AbilitySystemComponent�� ��ȯ�Ѵ�
	// - �츮�� DlkCharacter�� IAbilitySystemInterface�� ��ӹ��� �ʿ䰡 �ִ�
	return Cast<UDlkAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
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

UDlkEquipmentInstance* UDlkEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UDlkEquipmentInstance> InstanceType)
{
	for (FDlkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UDlkEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}
	return nullptr;
}

TArray<UDlkEquipmentInstance*> UDlkEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UDlkEquipmentInstance> InstanceType) const
{
	TArray<UDlkEquipmentInstance*> Results;

	// EquipmentList�� ��ȸ�ϸ�
	for (const FDlkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UDlkEquipmentInstance* Instance = Entry.Instance)
		{
			// InstanceType�� �´� Class�̸� Results�� �߰��Ͽ� ��ȯ
			// - �츮�� ���, DlkRangedWeaponInstance�� �ɰ���
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}
