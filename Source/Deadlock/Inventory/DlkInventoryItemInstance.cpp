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
