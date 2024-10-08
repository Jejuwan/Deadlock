// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkPawnData.generated.h"

class UDlkCameraMode;
/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Pawn¿« Class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dlk|Pawn")
	TSubclassOf<APawn> PawnClass;

	/** Camera Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dlk|Camera")
	TSubclassOf<UDlkCameraMode> DefaultCameraMode;
};
