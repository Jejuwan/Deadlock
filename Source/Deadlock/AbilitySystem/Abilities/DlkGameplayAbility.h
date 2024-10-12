// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DlkGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EDlkAbilityActivationPolicy : uint8
{
	/** Input�� Trigger �Ǿ��� ��� (Presssed/Released) */
	OnInputTriggered,
	/** Input�� Held�Ǿ� ���� ��� */
	WhileInputActive,
	/** avatar�� �����Ǿ��� ���, �ٷ� �Ҵ� */
	OnSpawn,
};

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UDlkGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** ���� GA�� Ȱ��ȭ���� ��å */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dlk|AbilityActivation")
	EDlkAbilityActivationPolicy ActivationPolicy;
};
