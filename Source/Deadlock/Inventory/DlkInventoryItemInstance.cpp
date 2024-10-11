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
		// DlkInventoryItemDefinition은 모든 멤버 변수가 EditDefaultsOnly로 선언되어 있으므로, GetDefault로 가져와도 무관하다
		// - Fragment 정보는 Instance가 아닌 Definition에 있다
		return GetDefault<UDlkInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
