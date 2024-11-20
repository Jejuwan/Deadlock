// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Deadlock/Equipment/DlkGameplayAbility_FromEquipment.h"
#include "DlkGameplayAbility_MeleeMinionAttack.generated.h"

class UDlkMeleeMinionAttackInstance;

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameplayAbility_MeleeMinionAttack : public UDlkGameplayAbility
{
	GENERATED_BODY()
public:
	struct FMeleeMinionAttackInput
	{
		FVector StartTrace;
		FVector EndAim;
		FQuat Rotation;

		FMeleeMinionAttackInput()
			: StartTrace(ForceInitToZero)
			, EndAim(ForceInitToZero)
		{}
	};

	UDlkGameplayAbility_MeleeMinionAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void StartMeleeMinionAttackTargeting();

	void PerformLocalTargeting(TArray<FHitResult>& OutHits);
	FRotator GetWeaponTargetingSourceRotation(FName SocketName) const;
	FVector GetWeaponTargetingSourceLocation(FName SocketName) const;
	FHitResult DoSingleBulletTrace(const FMeleeMinionAttackInput& InputData, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const;
	FHitResult WeaponTrace(const FMeleeMinionAttackInput& InputData, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const;
	ECollisionChannel DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;
	void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	/** called when target data is ready */
	UFUNCTION(BlueprintImplementableEvent)
	void OnMeleeMinionTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);


	int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResults) const;
public:
	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly)
	float WeaponHalfHeight;

	UPROPERTY(EditDefaultsOnly)
	float WeaponRadius;
};
