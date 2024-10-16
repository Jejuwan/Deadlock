// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "DlkAbilityCost.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class DEADLOCK_API UDlkAbilityCost : public UObject
{
	GENERATED_BODY()
public:
	UDlkAbilityCost();

	/**
	 * CheckCost and ApplyCost function signature come from UGameAbility's CheckCost and ApplyCost
	 * - You can think DlkAbilityCost as managable-unit to check/apply cost for GameplayAbility
	 */
	virtual bool CheckCost(const UDlkGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}

	virtual void ApplyCost(const UDlkGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
	{
	}
};
