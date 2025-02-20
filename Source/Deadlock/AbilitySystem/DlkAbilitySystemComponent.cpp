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
		// Notify all abilities that a new pawn avatar has been set
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UDlkGameplayAbility* DlkAbilityCDO = Cast<UDlkGameplayAbility>(AbilitySpec.Ability);
			if (!DlkAbilityCDO)
			{
				continue;
			}

			if (DlkAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : Instances)
				{
					UDlkGameplayAbility* DlkAbilityInstance = Cast<UDlkGameplayAbility>(AbilityInstance);
					if (DlkAbilityInstance)
					{
						// Ability instances may be missing for replays
						DlkAbilityInstance->OnPawnAvatarSet();
					}
				}
			}
			else
			{
				DlkAbilityCDO->OnPawnAvatarSet();
			}
		}

		if (UDlkAnimInstance* DlkAnimInst = Cast<UDlkAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			DlkAnimInst->InitializeWithAbilitySystem(this);
		}

		TryActivateAbilitiesOnSpawn();
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

void UDlkAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDlkAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDlkAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	//if (UDlkGameplayAbility* DlkAbility = Cast<UDlkGameplayAbility>(Ability))
	//{
	//	AddAbilityToActivationGroup(LyraAbility->GetActivationGroup(), LyraAbility);
	//}
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

void UDlkAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

void UDlkAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	if (const UDlkGameplayAbility* DlkAbility = Cast<const UDlkGameplayAbility>(Ability))
	{
		DlkAbility->OnAbilityFailedToActivate(FailureReason);
	}
}

void UDlkAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UDlkGameplayAbility* DlkAbilityCDO = Cast<UDlkGameplayAbility>(AbilitySpec.Ability))
		{
			DlkAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}
