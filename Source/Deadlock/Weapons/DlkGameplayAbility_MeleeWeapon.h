// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Deadlock/Equipment/DlkGameplayAbility_FromEquipment.h"
#include "DlkGameplayAbility_MeleeWeapon.generated.h"

class UDlkMeleeWeaponInstance;

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameplayAbility_MeleeWeapon : public UDlkGameplayAbility_FromEquipment
{
	GENERATED_BODY()
public:
	struct FMeleeWeaponFiringInput
	{
		FVector StartTrace;
		FVector EndAim;
		FVector AimDir;
		UDlkMeleeWeaponInstance* WeaponData = nullptr;
		bool bCanPlayBulletFX = false;

		FMeleeWeaponFiringInput()
			: StartTrace(ForceInitToZero)
			, EndAim(ForceInitToZero)
			, AimDir(ForceInitToZero)
		{}
	};

	UDlkGameplayAbility_MeleeWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void StartMeleeWeaponTargeting();

	void PerformLocalTargeting(TArray<FHitResult>& OutHits);
	FTransform GetTargetingTransform(APawn* SourcePawn, EDlkAbilityTargetingSource Source);
	FVector GetWeaponTargetingSourceLocation(FName SocketName) const;
	void TraceBulletsInCartridge(const FMeleeWeaponFiringInput& InputData, TArray<FHitResult>& OutHits);
	FHitResult DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const;
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const;
	ECollisionChannel DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;
	void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	/** called when target data is ready */
	UFUNCTION(BlueprintImplementableEvent)
	void OnRangeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UDlkMeleeWeaponInstance* GetWeaponInstance();


public:
	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName;
};
