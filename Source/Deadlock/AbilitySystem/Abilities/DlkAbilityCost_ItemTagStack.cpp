// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkAbilityCost_ItemTagStack.h"
#include "NativeGameplayTags.h"
#include "Deadlock/Equipment/DlkGameplayAbility_FromEquipment.h"
#include "Deadlock/Inventory/DlkInventoryItemInstance.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost")

UDlkAbilityCost_ItemTagStack::UDlkAbilityCost_ItemTagStack() : Super()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UDlkAbilityCost_ItemTagStack::CheckCost(const UDlkGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	// we only check the cost when item is equipped
	if (const UDlkGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UDlkGameplayAbility_FromEquipment>(Ability))
	{
		if (UDlkInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			// currently, it is just pistol (basic weapon)
			// to understand this a little bit deeply, we try to think weapon as magic pistol:
			// - the magic pistol costs two bullets, give a strong one shot when the weapon is lv2
			// - the magic pistol lv5 costs four bullets, give more strong one shot
			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

			// when we cannot be afford to give a shot, leave the failure tag in OptionalRelevantTags:
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);
			}

			return bCanApplyCost;
		}
	}
	return false;
}

void UDlkAbilityCost_ItemTagStack::ApplyCost(const UDlkGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (const UDlkGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UDlkGameplayAbility_FromEquipment>(Ability))
	{
		if (UDlkInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			// decrease amount of stat tags in an inventory item instance
			ItemInstance->RemoveStatTagStack(Tag, NumStacks);
		}
	}
}