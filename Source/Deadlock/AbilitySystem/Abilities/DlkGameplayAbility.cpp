#include "DlkGameplayAbility.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DlkAbilityCost.h"
#include "Deadlock/DlkGameplayTags.h"
#include "Deadlock/Character/DlkCharacter.h"
#include "Deadlock/Character/DlkHeroComponent.h"
#include "Deadlock/Player/DlkPlayerController.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "Deadlock/AbilitySystem/Abilities/DlkAbilitySimpleFailureMessage.h"
#include "Deadlock/AbilitySystem/DlkAbilitySourceInterface.h"
#include "Deadlock/AbilitySystem/DlkGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkGameplayAbility)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");

UDlkGameplayAbility::UDlkGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationPolicy = EDlkAbilityActivationPolicy::OnInputTriggered;
}

UDlkAbilitySystemComponent* UDlkGameplayAbility::GetDlkAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UDlkAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

ADlkPlayerController* UDlkGameplayAbility::GetDlkPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ADlkPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UDlkGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

ADlkCharacter* UDlkGameplayAbility::GetDlkCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ADlkCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UDlkHeroComponent* UDlkGameplayAbility::GetHeroComponentFromActorInfo() const
{
	return (CurrentActorInfo ? UDlkHeroComponent::FindHeroComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

bool UDlkGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// verify AdditionalCosts defined in DlkGameplayAbility to activate GameplayAbility:
	for (TObjectPtr<UDlkAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	// all cost requipements are meet! ready to activate!
	return true;
}

void UDlkGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	check(ActorInfo);

	// pay any additional cost
	for (TObjectPtr<UDlkAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

FGameplayEffectContextHandle UDlkGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);

	FDlkGameplayEffectContext* EffectContext = FDlkGameplayEffectContext::ExtractEffectContext(ContextHandle);
	check(EffectContext);

	check(ActorInfo);

	AActor* EffectCauser = nullptr;
	const IDlkAbilitySourceInterface* AbilitySource = nullptr;
	float SourceLevel = 0.0f;
	GetAbilitySource(Handle, ActorInfo, /*out*/ SourceLevel, /*out*/ AbilitySource, /*out*/ EffectCauser);

	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;

	EffectContext->SetAbilitySource(AbilitySource, SourceLevel);
	EffectContext->AddInstigator(Instigator, EffectCauser);
	EffectContext->AddSourceObject(SourceObject);

	return ContextHandle;
}

void UDlkGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IDlkAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
{
	OutSourceLevel = 0.0f;
	OutAbilitySource = nullptr;
	OutEffectCauser = nullptr;

	OutEffectCauser = ActorInfo->AvatarActor.Get();

	// If we were added by something that's an ability info source, use it
	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	OutAbilitySource = Cast<IDlkAbilitySourceInterface>(SourceObject);
}

void UDlkGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	//TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UDlkGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	bool bSimpleFailureFound = false;
	for (FGameplayTag Reason : FailedReason)
	{
		if (!bSimpleFailureFound)
		{
			if (const FText* pUserFacingMessage = FailureTagToUserFacingMessages.Find(Reason))
			{
				FDlkAbilitySimpleFailureMessage Message;
				Message.PlayerController = GetActorInfo().PlayerController.Get();
				Message.FailureTags = FailedReason;
				Message.UserFacingReason = *pUserFacingMessage;

				UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
				MessageSystem.BroadcastMessage(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, Message);
				bSimpleFailureFound = true;
			}
		}

		if (UAnimMontage* pMontage = FailureTagToAnimMontage.FindRef(Reason))
		{
			FDlkAbilityMontageFailureMessage Message;
			Message.PlayerController = GetActorInfo().PlayerController.Get();
			Message.FailureTags = FailedReason;
			Message.FailureMontage = pMontage;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, Message);
		}
	}
}
