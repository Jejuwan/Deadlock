// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DlkEquipmentDefinition.generated.h"

class UDlkEquipmentInstance;
class UDlkAbilitySet;

USTRUCT()
struct FDlkEquipmentActorToSpawn
{
	GENERATED_BODY()

	/** Spawn�� ��� Actor Ŭ���� (== Actor�� ��ӹ��� Asset���� �����ص� ��) */
	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	/** ��� Bone Socket�� ������ �����Ѵ� */
	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	/** Socket���� ������� Transformation�� ���Ұ����� ����: (Rotation, Position, Scale) */
	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;
};

/**
 * DlkEquipementDefinition�� ���� �����ۿ� ���� ���� Ŭ����(��Ÿ ������)�̴�
 */
UCLASS(Blueprintable)
class DEADLOCK_API UDlkEquipmentDefinition : public UObject
{
	GENERATED_BODY()
public:
	UDlkEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** �ش� ��Ÿ �����͸� ����ϸ�, � �ν��Ͻ��� Spawn���� �����ϴ� Ŭ���� */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TSubclassOf<UDlkEquipmentInstance> InstanceType;

	/** �ش� ���� �������� ����ϸ�, � Actor�� Spawn�� �Ǵ��� ������ ��� �ִ� */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FDlkEquipmentActorToSpawn> ActorsToSpawn;

	/** ������ ���� �ο� ������ Ability Set */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<TObjectPtr<UDlkAbilitySet>> AbilitySetsToGrant;
};
