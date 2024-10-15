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

	// EquipmentDefinition의 멤버 변수들은 EditDefaultsOnly로 정의되어 있어 GetDefault로 들고 와도 우리에게 필요한 것들이 모두 들어있다
	const UDlkEquipmentDefinition* EquipmentCDO = GetDefault<UDlkEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UDlkEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UDlkEquipmentInstance::StaticClass();
	}

	// Entries에 추가해주자
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

	// ActorsToSpawn을 통해, Actor들을 인스턴스화 해주자
	// - 어디에? EquipmentInstance에!
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	return Result;
}

void FDlkEquipmentList::RemoveEntry(UDlkEquipmentInstance* Instance)
{
	// 단순히 그냥 Entries를 순회하며, Instance를 찾아서
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FDlkAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			UDlkAbilitySystemComponent* ASC = GetAbilitySystemComponent();
			check(ASC);
			{
				// TakeFromAbilitySystem은 GiveToAbilitySystem 반대 역활로, ActivatableAbilities에서 제거한다
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			// Actor 제거 작업 및 iterator를 통한 안전하게 Array에서 제거 진행
			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

UDlkAbilitySystemComponent* FDlkEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();

	// GetAbilitySystemComponentFromActor를 잠시 확인해보자:
	// - EquipmentManagerComponent는 ADlkCharacter를 Owner로 가지고 있다
	// - 해당 함수는 IAbilitySystemInterface를 통해 AbilitySystemComponent를 반환한다
	// - 우리는 DlkCharacter에 IAbilitySystemInterface를 상속받을 필요가 있다
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
		// 해당 함수는 BP의 Event노드를 호출해준다 (자세한건 해당 함수 구현하면서 보자)
		ItemInstance->OnUnequipped();

		// EquipementList에 제거해준다:
		// - 제거하는 과정을 통해 추가되었던 Actor Instance를 제거를 진행한다
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

	// EquipmentList를 순회하며
	for (const FDlkAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UDlkEquipmentInstance* Instance = Entry.Instance)
		{
			// InstanceType에 맞는 Class이면 Results에 추가하여 반환
			// - 우리의 경우, DlkRangedWeaponInstance가 될거임
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}
