// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Deadlock/AbilitySystem/DlkAbilitySet.h"
#include "DlkEquipmentManagerComponent.generated.h"

/** forward declarations */
class UDlkEquipmentDefinition;
class UDlkEquipmentInstance;

USTRUCT(BlueprintType)
struct FDlkAppliedEquipmentEntry
{
	GENERATED_BODY()

	/** �������� ���� ��Ÿ ������ */
	UPROPERTY()
	TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition;

	/** EquipmentDefinition�� ���� �������� �ν��Ͻ� */
	UPROPERTY()
	TObjectPtr<UDlkEquipmentInstance> Instance = nullptr;

	/** ���⿡ �Ҵ�� ��밡���� GameplayAbility */
	UPROPERTY()
	FDlkAbilitySet_GrantedHandles GrantedHandles;
};

/**
 * ����� EquipmentInstance�� �ν��Ͻ��� Entry���� �����ϰ� �ִ�:
 * - DlkEquipmentList�� ������ ��ü�� �����Ѵٰ� ���� �ȴ�
 */
USTRUCT(BlueprintType)
struct FDlkEquipmentList
{
	GENERATED_BODY()

	FDlkEquipmentList(UActorComponent* InOwnerComponent = nullptr)
		: OwnerComponent(InOwnerComponent)
	{}

	UDlkEquipmentInstance* AddEntry(TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UDlkEquipmentInstance* Instance);

	UDlkAbilitySystemComponent* GetAbilitySystemComponent() const;

	/** �������� ���� ���� ����Ʈ */
	UPROPERTY()
	TArray<FDlkAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * Pawn�� Component�μ� �������� ���� ������ ����Ѵ�
 */
UCLASS()
class DEADLOCK_API UDlkEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UDlkEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UDlkEquipmentInstance* EquipItem(TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition);
	void UnequipItem(UDlkEquipmentInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	TArray<UDlkEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UDlkEquipmentInstance> InstanceType) const;

	UPROPERTY()
	FDlkEquipmentList EquipmentList;
};
