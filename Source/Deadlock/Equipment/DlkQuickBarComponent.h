// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "DlkQuickBarComponent.generated.h"

class UDlkEquipmentManagerComponent;
class UDlkEquipmentInstance;
class UDlkInventoryItemInstance;

/**
 * HUD�� QuckBar�� �����ϸ� �ȴ�:
 * - ���� MMORPG������ ShortCut HUD�� �����ϸ� �ȴ�
 *
 * �ش� Component�� ControllerComponent�μ�, PlayerController�� ���� ���۰� �� �ϳ��� �����ص� �ȴ�
 * - HUD(Slate)�� Inventory/Equipment(Gameplay)�� �ٸ�(Bridge) ��Ȱ�ϴ� Component�� ��������
 * - �ش� Component�� Lyra�� HUD �� Slate Widget�� �ٷ�鼭 �ٽ� ���Ե� �����̴�
 */
UCLASS()
class DEADLOCK_API UDlkQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UDlkQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* ControllerComponent interface
	*/
	virtual void BeginPlay() override;

	/**
	* member methods
	*/
	UDlkEquipmentManagerComponent* FindEquipmentManager() const;
	void UnequipItemInSlot();
	void EquipItemInSlot();

	UFUNCTION(BlueprintCallable)
	void AddItemToSlot(int32 SlotIndex, UDlkInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UDlkInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Dlk")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<UDlkInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UDlkInventoryItemInstance* GetActiveSlotItem() const;

	/** HUD QuickBar Slot ���� */
	UPROPERTY()
	int32 NumSlots = 3;

	/** HUD QuickBar Slot ����Ʈ */
	UPROPERTY()
	TArray<TObjectPtr<UDlkInventoryItemInstance>> Slots;

	/** ���� Ȱ��ȭ�� Slot Index (�Ƹ� Lyra�� �� �Ѱ��� Slot�� Ȱ��ȭ�Ǵ°�����?) */
	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	/** ���� ������ ��� ���� */
	UPROPERTY()
	TObjectPtr<UDlkEquipmentInstance> EquippedItem;
};
