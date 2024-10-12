// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "DlkAbilitySet.generated.h"

class UDlkAbilitySystemComponent;
class UDlkGameplayAbility;

/**
 * GameplayAbility�� Wrapper Ŭ����
 * - �߰����� Ŀ���͸���¡�� ������
 */
USTRUCT(BlueprintType)
struct FDlkAbilitySet_GameplayAbility
{
	GENERATED_BODY()
public:
	/** ���� GameplayAbility */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDlkGameplayAbility> Ability = nullptr;

	/** Input ó���� ���� GameplayTag */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;

	/** Ability�� ��� ���� (Level) */
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

USTRUCT(BlueprintType)
struct FDlkAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void TakeFromAbilitySystem(UDlkAbilitySystemComponent* DlkASC);

protected:
	/** ���� GameplayAbilitySpecHandle(int32) */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** ASC�� ��밡���� Ability�� �߰��Ѵ� */
	void GiveToAbilitySystem(UDlkAbilitySystemComponent* DlkASC, FDlkAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr);

	/** ���� GameplayAbilities */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FDlkAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
