// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkInventoryItemInstance.h"

#include "DlkInventoryItemDefinition.h"

UDlkInventoryItemInstance::UDlkInventoryItemInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UDlkInventoryItemFragment* UDlkInventoryItemInstance::FindFragmentByClass(TSubclassOf<UDlkInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		// DlkInventoryItemDefinition�� ��� ��� ������ EditDefaultsOnly�� ����Ǿ� �����Ƿ�, GetDefault�� �����͵� �����ϴ�
		// - Fragment ������ Instance�� �ƴ� Definition�� �ִ�
		return GetDefault<UDlkInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

void UDlkInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UDlkInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

bool UDlkInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

int32 UDlkInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}
