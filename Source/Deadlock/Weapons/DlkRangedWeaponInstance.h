// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkWeaponInstance.h"
#include "DlkRangedWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkRangedWeaponInstance : public UDlkWeaponInstance
{
	GENERATED_BODY()
public:
	/** ��ȿ ��Ÿ� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponConfig", meta = (ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

	/** ��ź ���� (Sphere Trace Sweep) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponConfig", meta = (ForceUnits = cm))
	float BulletTraceWeaponRadius = 0.0f;
};
