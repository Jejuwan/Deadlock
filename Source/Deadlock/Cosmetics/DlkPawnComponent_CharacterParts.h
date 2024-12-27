// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkCharacterPartTypes.h"
#include "DlkCosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "DlkPawnComponent_CharacterParts.generated.h"

class UDlkPawnComponent_CharacterParts;

/** 인스턴스화 된 Character Part의 단위 */
USTRUCT()
struct FDlkAppliedCharacterPartEntry
{
	GENERATED_BODY()

	/** Character Part의 정의(메타 데이터) */
	UPROPERTY()
	FDlkCharacterPart Part;

	/** DlkCharacterPartList에서 할당 받은 Part 핸들값 (FDlkControllerCharacterPartEntry의 Handle 값과 같아야 함 -> 같으면 같은 Part) */
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	/** 인스턴스화 된 Character Part용 Actor */
	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

/** DlkPawnComponent_CharacterParts에서 실질적 Character Parts를 관리하는 클래스 */
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

	/** 현재 인스턴스화된 Character Part */
	UPROPERTY()
	TArray<FDlkAppliedCharacterPartEntry> Entries;

	/** 해당 DlkCharacterPartList의 Owner인 PawnComponent */
	UPROPERTY()
	TObjectPtr<UDlkPawnComponent_CharacterParts> OwnerComponent;

	/** 앞서 보았던 PartHandle의 값을 할당 및 관리하는 변수 */
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



	/** 인스턴스화 된 Character Parts */
	UPROPERTY()
	FDlkCharacterPartList CharacterPartList;

	/** 애니메이션 적용을 위한 메시와 연결고리 */
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	FDlkAnimBodyStyleSelectionSet BodyMeshes;
};
