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
	// �ش� property�� �ܼ��� ��ŷ �� �������� ���ܳ����� �ϰڴ�:
	// - GameMode�� ���� �ʿ��� GameFeature plugin���� �ε��ϴµ� �̿� ���� ������� �����ϸ� �ȴ� (����� ���� ����)
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
};
