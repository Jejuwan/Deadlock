// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkPawnData.generated.h"

class UDlkInputConfig;
class UDlkCameraMode;
class UDlkAbilitySet;
/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Pawn의 Class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dlk|Pawn")
	TSubclassOf<APawn> PawnClass;

	/** Camera Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dlk|Camera")
	TSubclassOf<UDlkCameraMode> DefaultCameraMode;

	/** input configuration used by player controlled pawns to create input mappings and bind input actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dlk|InputConfig")
	TObjectPtr<UDlkInputConfig> InputConfig;

	/** 해당 Pawn의 Ability System에 허용할 AbilitySet */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hak|Abilities")
	TArray<TObjectPtr<UDlkAbilitySet>> AbilitySets;
};
