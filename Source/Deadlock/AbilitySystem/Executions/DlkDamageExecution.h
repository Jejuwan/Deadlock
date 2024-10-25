// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "DlkDamageExecution.generated.h"

class UObject;


/**
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UDlkDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UDlkDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
