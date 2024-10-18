// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkAbilitySystemComponent.h"

#include "Abilities/DlkGameplayAbility.h"
#include "GameFramework/Pawn.h"
#include "Deadlock/Animation/DlkAnimInstance.h"

UDlkAbilitySystemComponent::UDlkAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDlkAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		if (UDlkAnimInstance* LyraAnimInst = Cast<UDlkAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			LyraAnimInst->InitializeWithAbilitySystem(this);
		}
	}
}

void UDlkAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		// ���� GameplayAbilitySpec�� ��ȸ
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			// Ability�� �����ϰ�, DynamicAbilityTags�� InputTag�� ���� ���, InputPressed/Held�� �־� Ability ó���� ����Ѵ�
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UDlkAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				// Released�� �߰��ϰ�, Held������ �������ش�
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UDlkAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// InputHeldSpecHandles�� ���� Ability ó���� ���� AbilitiesToActivate�� �߰��Ѵ�
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		// FindAbilitySpecFromHandle Ȯ��:
		// - ActivatableAbilities�� Handle �� �񱳸� ���� GameplayAbilitySpec�� ��ȯ�Ѵ�
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UDlkGameplayAbility* DlkAbilityCDO = CastChecked<UDlkGameplayAbility>(AbilitySpec->Ability);

				// ActivationPolicy�� WhileInputActive �Ӽ��̸� Ȱ��ȭ�� ���
				if (DlkAbilityCDO->ActivationPolicy == EDlkAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// �̹� Ability�� Ȱ��ȭ�Ǿ� ���� ���, Input Event(InputPressed)�� ȣ��
					// - AbilitySpecInputPressed Ȯ��
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UDlkGameplayAbility* DlkAbilityCDO = CastChecked<UDlkGameplayAbility>(AbilitySpec->Ability);

					// ActivationPolicy�� OnInputTriggered �Ӽ��̸� Ȱ��ȭ�� ���
					if (DlkAbilityCDO->ActivationPolicy == EDlkAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// ��ϵ� AbilitiesToActivate�� �Ѳ����� ��� ����:
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		// ��� ���� �� ����Ǿ��ٸ�, CallActivate ȣ��� BP�� Activate ��尡 ����� ����
		TryActivateAbility(AbilitySpecHandle);
	}

	// �̹� �����ӿ� Release�Ǿ��ٸ�, ���� GameplayAbility ó��:
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// InputHeldSpecHandles�� InputReleasedSpecHandles �߰��ɶ� ���ŵȴ�!
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UDlkAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	/*if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}*/

	HandleAbilityFailed(Ability, FailureReason);
}

//void UDlkAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
//{
//	HandleAbilityFailed(Ability, FailureReason);
//}

void UDlkAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	if (const UDlkGameplayAbility* LyraAbility = Cast<const UDlkGameplayAbility>(Ability))
	{
		LyraAbility->OnAbilityFailedToActivate(FailureReason);
	}
}
