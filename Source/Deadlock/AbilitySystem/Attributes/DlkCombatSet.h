// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DlkAttributeSet.h"
#include "DlkCombatSet.generated.h"

/**
 * CombatSet�� �̸� �״��, ������ ���õ� Attribute�� ��� �ִ� Set�̴�:
 * - ����� BaseHeal�� ������, BaseDamage�� �߰��Ͽ�, ������ CombatSet�� �ʿ��� AttributeSet�� ���� �����ϴ�
 */
UCLASS()
class DEADLOCK_API UDlkCombatSet : public UDlkAttributeSet
{
	GENERATED_BODY()
public:
	UDlkCombatSet();

	ATTRIBUTE_ACCESSORS(UDlkCombatSet, BaseHeal);

	/**
	 * FGameplayAttribute�� �����ϴ� ���� AttributeSet�� �ִ� �������̴� (float���� �ش� Struct�� ����ϴ� ���� ��õ)
	 * - Healing�� ������ �ǹ��Ѵ�
	 * - e.g. 5.0f���, Period�� 5�� Healing�ȴٴ� �ǹ�
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Combat")
	FGameplayAttributeData BaseHeal;
};
