// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkInventoryManagerComponent.h"
#include "DlkInventoryItemDefinition.h"
#include "DlkInventoryItemInstance.h"

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
