// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkInventoryItemDefinition.h"

UDlkInventoryItemDefinition::UDlkInventoryItemDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UDlkInventoryItemFragment* UDlkInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UDlkInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		// Fragments�� ��ȸ�ϸ�, IsA()�� ���� �ش� Ŭ������ ������ �ִ��� Ȯ���Ѵ�:
		for (UDlkInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}
