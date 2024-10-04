// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "DlkExperienceDefinition.generated.h"

class UDlkPawnData;

UCLASS()
class DEADLOCK_API UDlkExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** the default pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UDlkPawnData> DefaultPawnData;

	/** lost if game feature plugins this experience wants to have active */
	// 해당 property는 단순히 마킹 및 기억용으로 남겨놓도록 하겠다:
	// - GameMode에 따라 필요한 GameFeature plugin들을 로딩하는데 이에 대한 연결고리로 생각하면 된다 (현재는 쓰지 않음)
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
};
