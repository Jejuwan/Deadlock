// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DlkInventoryManagerComponent.generated.h"

/** forward declarations */
class UDlkInventoryItemInstance;
class UDlkInventoryItemDefinition;

/** Inventory Item ���� ��ü */
USTRUCT(BlueprintType)
struct FDlkInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UDlkInventoryItemInstance> Instance = nullptr;
};

/** Inventory Item ���� ��ü */
USTRUCT(BlueprintType)
struct FDlkInventoryList
{
	GENERATED_BODY()

	FDlkInventoryList(UActorComponent* InOwnerComponent = nullptr) : OwnerComponent(InOwnerComponent)
	{}

	UDlkInventoryItemInstance* AddEntry(TSubclassOf<UDlkInventoryItemDefinition> ItemDef);

	UPROPERTY()
	TArray<FDlkInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * PlayerController�� Component�μ� Inventory�� �����Ѵ�
 * - ��� UActorComponent ����� �ƴ� UControllerComponent�� ��ӹ޾Ƶ� �ɰ� ������... �ϴ� Lyra �������� UActorComponent�� ��ӹް� �ִ�
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADLOCK_API UDlkInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDlkInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** InventoryItemDefinition�� ����, InventoryList�� �߰��Ͽ� �����ϸ�, InventoryItemInstance�� ��ȯ�Ѵ� */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UDlkInventoryItemInstance* AddItemDefinition(TSubclassOf<UDlkInventoryItemDefinition> ItemDef);

	UPROPERTY()
	FDlkInventoryList InventoryList;
};
