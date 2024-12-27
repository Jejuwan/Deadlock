// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkCharacterPartTypes.h"
#include "DlkCosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "DlkPawnComponent_CharacterParts.generated.h"

class UDlkPawnComponent_CharacterParts;

/** �ν��Ͻ�ȭ �� Character Part�� ���� */
USTRUCT()
struct FDlkAppliedCharacterPartEntry
{
	GENERATED_BODY()

	/** Character Part�� ����(��Ÿ ������) */
	UPROPERTY()
	FDlkCharacterPart Part;

	/** DlkCharacterPartList���� �Ҵ� ���� Part �ڵ鰪 (FDlkControllerCharacterPartEntry�� Handle ���� ���ƾ� �� -> ������ ���� Part) */
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	/** �ν��Ͻ�ȭ �� Character Part�� Actor */
	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

/** DlkPawnComponent_CharacterParts���� ������ Character Parts�� �����ϴ� Ŭ���� */
USTRUCT(BlueprintType)
struct FDlkCharacterPartList
{
	GENERATED_BODY()

	FDlkCharacterPartList()
		: OwnerComponent(nullptr)
	{}

	FDlkCharacterPartList(UDlkPawnComponent_CharacterParts* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	bool SpawnActorForEntry(FDlkAppliedCharacterPartEntry& Entry);
	void DestroyActorForEntry(FDlkAppliedCharacterPartEntry& Entry);

	FDlkCharacterPartHandle AddEntry(FDlkCharacterPart NewPart);
	void RemoveEntry(FDlkCharacterPartHandle Handle);

	FGameplayTagContainer CollectCombinedTags() const;

	/** ���� �ν��Ͻ�ȭ�� Character Part */
	UPROPERTY()
	TArray<FDlkAppliedCharacterPartEntry> Entries;

	/** �ش� DlkCharacterPartList�� Owner�� PawnComponent */
	UPROPERTY()
	TObjectPtr<UDlkPawnComponent_CharacterParts> OwnerComponent;

	/** �ռ� ���Ҵ� PartHandle�� ���� �Ҵ� �� �����ϴ� ���� */
	int32 PartHandleCounter = 0;
};

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkPawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()
public:
	UDlkPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USkeletalMeshComponent* GetParentMeshComponent() const;
	USceneComponent* GetSceneComponentToAttachTo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;
	void BroadcastChanged();

	FDlkCharacterPartHandle AddCharacterPart(const FDlkCharacterPart& NewPart);
	void RemoveCharacterPart(FDlkCharacterPartHandle Handle);



	/** �ν��Ͻ�ȭ �� Character Parts */
	UPROPERTY()
	FDlkCharacterPartList CharacterPartList;

	/** �ִϸ��̼� ������ ���� �޽ÿ� ����� */
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	FDlkAnimBodyStyleSelectionSet BodyMeshes;
};
