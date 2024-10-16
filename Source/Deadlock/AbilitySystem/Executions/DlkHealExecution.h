// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DlkHealExecution.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UDlkHealExecution();

	/** 해당 메서드는 GameplayEffectExecutionCalculation의 Execute() BlueprintNativeEvent를 오버라이드 한다 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
