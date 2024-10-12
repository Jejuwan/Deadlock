// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameplayAbility.h"

UDlkGameplayAbility::UDlkGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EDlkAbilityActivationPolicy::OnInputTriggered;
}