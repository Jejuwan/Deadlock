// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "DlkWeaponUserInterface.generated.h"

/** forward declaration */
class UDlkWeaponInstance;

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UDlkWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Weapon ���Ͽ� ���� BP Event */
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UDlkWeaponInstance* OldWeapon, UDlkWeaponInstance* NewWeapon);

	/**
	 * UUserWidget's interface
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** ���� ������ WeaponInstance�� �����Ѵ� (NativeTick�� Ȱ���Ͽ� �ֱ��� ������Ʈ�Ѵ�) */
	UPROPERTY(Transient)
	TObjectPtr<UDlkWeaponInstance> CurrentInstance;
};
