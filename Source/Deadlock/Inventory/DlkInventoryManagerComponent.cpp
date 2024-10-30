// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkInventoryManagerComponent.h"
#include "DlkInventoryItemDefinition.h"
#include "DlkInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

UDlkInventoryItemInstance* FDlkInventoryList::AddEntry(TSubclassOf<UDlkInventoryItemDefinition> ItemDef)
{
	UDlkInventoryItemInstance* Result = nullptr;
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FDlkInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UDlkInventoryItemInstance>(OwningActor);
	NewEntry.Instance->ItemDef = ItemDef;

	// iterating fragments and call callback to OnInstanceCreated()
	for (UDlkInventoryItemFragment* Fragment : GetDefault<UDlkInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}


	Result = NewEntry.Instance;
	return Result;
}

TArray<UDlkInventoryItemInstance*> FDlkInventoryList::GetAllItems() const
{
	TArray<UDlkInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FDlkInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

void FDlkInventoryList::RemoveEntry(UDlkInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FDlkInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			//MarkArrayDirty();
		}
	}
}

UDlkInventoryManagerComponent::UDlkInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{}

UDlkInventoryItemInstance* UDlkInventoryManagerComponent::AddItemDefinition(TSubclassOf<UDlkInventoryItemDefinition> ItemDef)
{
	UDlkInventoryItemInstance* Result = nullptr;
	if (ItemDef)
	{
		Result = InventoryList.AddEntry(ItemDef);
	}
	return Result;
}

void UDlkInventoryManagerComponent::RemoveItemInstance(UDlkInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UDlkInventoryItemInstance*> UDlkInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}
