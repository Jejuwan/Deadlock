#include "DlkGameplayAbility.h"
#include "DlkAbilityCost.h"
#include "Deadlock/DlkGameplayTags.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkGameplayAbility)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");

UDlkGameplayAbility::UDlkGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationPolicy = EDlkAbilityActivationPolicy::OnInputTriggered;
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

void UDlkGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	//bool bSimpleFailureFound = false;
	//for (FGameplayTag Reason : FailedReason)
	//{
	//	if (!bSimpleFailureFound)
	//	{
	//		if (const FText* pUserFacingMessage = FailureTagToUserFacingMessages.Find(Reason))
	//		{
	//			FLyraAbilitySimpleFailureMessage Message;
	//			Message.PlayerController = GetActorInfo().PlayerController.Get();
	//			Message.FailureTags = FailedReason;
	//			Message.UserFacingReason = *pUserFacingMessage;

	//			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	//			MessageSystem.BroadcastMessage(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, Message);
	//			bSimpleFailureFound = true;
	//		}
	//	}

	//	if (UAnimMontage* pMontage = FailureTagToAnimMontage.FindRef(Reason))
	//	{
	//		FLyraAbilityMontageFailureMessage Message;
	//		Message.PlayerController = GetActorInfo().PlayerController.Get();
	//		Message.FailureTags = FailedReason;
	//		Message.FailureMontage = pMontage;

	//		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	//		MessageSystem.BroadcastMessage(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, Message);
	//	}
	//}
}
