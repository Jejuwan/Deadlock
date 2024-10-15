// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DlkInventoryItemInstance.generated.h"

class UDlkInventoryItemFragment;
class UDlkInventoryItemDefinition;

/**
 * 
 */
UCLASS(BlueprintType)
class DEADLOCK_API UDlkInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	UDlkInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UDlkInventoryItemFragment* FindFragmentByClass(TSubclassOf<UDlkInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}


	/** Inventory Item�� �ν��Ͻ����� �������� ���ǵǾ����� ��Ÿ Ŭ������ DlkInventoryItemDefinition�� ��� �ִ� */
	UPROPERTY()
	TSubclassOf<UDlkInventoryItemDefinition> ItemDef;
};
