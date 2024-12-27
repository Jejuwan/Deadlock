// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkAbilitySet.h"
#include "DlkAbilitySystemComponent.h"
#include "Abilities/DlkGameplayAbility.h"

void FDlkAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FDlkAbilitySet_GrantedHandles::TakeFromAbilitySystem(UDlkAbilitySystemComponent* DlkASC)
{
	if (!DlkASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			// ActivatableAbilities���� �����Ѵ�:
			// - ClearAbility() �Լ��� ��� ���� ����
			DlkASC->ClearAbility(Handle);
		}
	}
}

UDlkAbilitySet::UDlkAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UDlkAbilitySet::GiveToAbilitySystem(UDlkAbilitySystemComponent* DlkASC, FDlkAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(DlkASC);

	if (!DlkASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// gameplay abilities�� ���:
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FDlkAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		// GiveAbility()���� ���� EGameplayAbilityInstancingPolicy::InstancedPerActor���, ���������� Instance�� ���������� �׷��� ������ CDO�� �Ҵ��Ѵ�
		// - �̸� ���� UObject ������ �ٿ� UObject�� �þ�� ���� �þ�� ������/�޸��� ���ϸ� ���� �� �ִ�
		UDlkGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UDlkGameplayAbility>();

		// AbilitySpec�� GiveAbility�� ���޵Ǿ�, ActivatbleAbilities�� �߰��ȴ�
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// GiveAbility()�� ��� ���캸���� ����:
		const FGameplayAbilitySpecHandle AbilitySpecHandle = DlkASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}

