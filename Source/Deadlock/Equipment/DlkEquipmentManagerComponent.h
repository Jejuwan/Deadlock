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

	/** 장착물에 대한 메타 데이터 */
	UPROPERTY()
	TSubclassOf<UDlkEquipmentDefinition> EquipmentDefinition;

	/** EquipmentDefinition을 통해 생성도니 인스턴스 */
	UPROPERTY()
	TObjectPtr<UDlkEquipmentInstance> Instance = nullptr;

	/** 무기에 할당된 허용가능한 GameplayAbility */
	UPROPERTY()
	FDlkAbilitySet_GrantedHandles GrantedHandles;
};

/**
 * 참고로 EquipmentInstance의 인스턴스를 Entry에서 관리하고 있다:
 * - DlkEquipmentList는 생성된 객체를 관리한다고 보면 된다
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

	/** 장착물에 대한 관리 리스트 */
	UPROPERTY()
	TArray<FDlkAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * Pawn의 Component로서 장착물에 대한 관리를 담당한다
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
