// Copyright Epic Games, Inc. All Rights Reserved.

#include "DlkAbilitySystemGlobals.h"

#include "DlkGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkAbilitySystemGlobals)

struct FGameplayEffectContext;

UDlkAbilitySystemGlobals::UDlkAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UDlkAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FDlkGameplayEffectContext();
}

