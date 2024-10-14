// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkExperienceDefinition.generated.h"

class UDlkPawnData;
class UDlkExperienceActionSet;
class UGameFeatureAction;

UCLASS()
class DEADLOCK_API UDlkExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	/** the default pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UDlkPawnData> DefaultPawnData;

	/** lost if game feature plugins this experience wants to have active */
	// 해당 property는 단순히 마킹 및 기억용으로 남겨놓도록 하겠다:
	// - GameMode에 따라 필요한 GameFeature plugin들을 로딩하는데 이에 대한 연결고리로 생각하면 된다 (현재는 쓰지 않음)
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** ExperienceActionSet은 UGameFeatureAction의 Set이며, Gameplay 용도에 맞게 분류의 목적으로 사용한다 */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<UDlkExperienceActionSet>> ActionSets;

	/** 일반적인 GameFeatureAction으로서 추가 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
