// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DlkInventoryManagerComponent.generated.h"

/** forward declarations */
class UDlkInventoryItemInstance;
class UDlkInventoryItemDefinition;

/** Inventory Item 단위 객체 */
USTRUCT(BlueprintType)
struct FDlkInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UDlkInventoryItemInstance> Instance = nullptr;
};

/** Inventory Item 관리 객체 */
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
 * PlayerController의 Component로서 Inventory를 관리한다
 * - 사실 UActorComponent 상속이 아닌 UControllerComponent를 상속받아도 될거 같은데... 일단 Lyra 기준으로 UActorComponent를 상속받고 있다
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADLOCK_API UDlkInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDlkInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** InventoryItemDefinition을 통해, InventoryList에 추가하여 관리하며, InventoryItemInstance를 반환한다 */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UDlkInventoryItemInstance* AddItemDefinition(TSubclassOf<UDlkInventoryItemDefinition> ItemDef);

	UPROPERTY()
	FDlkInventoryList InventoryList;
};
