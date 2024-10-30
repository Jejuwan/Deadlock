// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkWeaponInstance.h"
#include "Deadlock/Character/DlkHealthComponent.h"

UDlkWeaponInstance::UDlkWeaponInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Listen for death of the owning pawn so that any device properties can be removed if we
	// die and can't unequip
	if (APawn* Pawn = GetPawn())
	{
		// We only need to do this for player controlled pawns, since AI and others won't have input devices on the client
		if (Pawn->IsPlayerControlled())
		{
			if (UDlkHealthComponent* HealthComponent = UDlkHealthComponent::FindHealthComponent(GetPawn()))
			{
				HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
			}
		}
	}
}

TSubclassOf<UAnimInstance> UDlkWeaponInstance::PickBestAnimLayer(bool bEquipped,
	const FGameplayTagContainer& CosmeticTags) const
{
	const FDlkAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnequippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}

const FPlatformUserId UDlkWeaponInstance::GetOwningUserId() const
{
	if (const APawn* Pawn = GetPawn())
	{
		return Pawn->GetPlatformUserId();
	}
	return PLATFORMUSERID_NONE;
}

void UDlkWeaponInstance::OnDeathStarted(AActor* OwningActor)
{
	// Remove any possibly active device properties when we die to make sure that there aren't any lingering around
	RemoveDeviceProperties();
}

void UDlkWeaponInstance::RemoveDeviceProperties()
{
	//const FPlatformUserId UserId = GetOwningUserId();

	//if (UserId.IsValid() && !DevicePropertyHandles.IsEmpty())
	//{
	//	// Remove any device properties that have been applied
	//	if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
	//	{
	//		InputDeviceSubsystem->RemoveDevicePropertyHandles(DevicePropertyHandles);
	//		DevicePropertyHandles.Empty();
	//	}
	//}
}

