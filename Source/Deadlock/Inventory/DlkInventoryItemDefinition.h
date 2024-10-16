// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DlkInventoryItemDefinition.generated.h"

class UDlkInventoryItemInstance;

/**
 * Inventory�� ���� Fragment�� Ȯ �ʹ��� ���� �� �ִ�:
 * - Lyra���� ����ϴ� ���ø� ���� �����غ���:
 *   - ULyraInventoryFragment_EquippableItem�� EquipmentItemDefinition�� ������ ������, ���� ������ �������� �ǹ��Ѵ�
 *   - ULyraInventoryFramgent_SetStats�� �����ۿ� ���� ������ ������ �ִ�
 *     - Rifle�� ���� SetStats���� �Ѿ�(Ammo)�� ���� ���� �ִ�ġ�� ���� ���� ��ź ���� ���÷� �� �� �ִ�
 *	 - ���...
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UDlkInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
	/** interface to call when inventory item instance is added to UHakInventoryManagerComponent's InventoryList */
	virtual void OnInstanceCreated(UDlkInventoryItemInstance* Instance) const {}
};

/**
 * 
 */
UCLASS(Blueprintable)
class DEADLOCK_API UDlkInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	UDlkInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UDlkInventoryItemFragment* FindFragmentByClass(TSubclassOf<UDlkInventoryItemFragment> FragmentClass) const;

	/** Inventory Item ����(��Ÿ) �̸� **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	/** Inventory Item�� Component�� Fragment�� �ν��ϸ� �ȴ� */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = Display)
	TArray<TObjectPtr<UDlkInventoryItemFragment>> Fragments;
};
