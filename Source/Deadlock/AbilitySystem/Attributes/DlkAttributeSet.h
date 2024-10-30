// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "DlkAttributeSet.generated.h"

struct FGameplayEffectSpec;

/**
 * �Ʒ� ��ũ�δ� AttributeSet�� Attribute�� �߰��� ��, ���� �� �����ؾ��� �޼��忡 ���� ���������� �����Ѵ�:
 *
 * ATTRIBUTE_ACCESSORS(UDlkHealthSet, Health):
 * �̴� �Ʒ��� �޼��带 ���� �� �������ش�
 *
 *   static FGameplayAttribute GetHealthAttribute() {...}
 *   float GetHealth() const {...}
 *   void SetHealth(float NewVal) {...}
 *   void InitHealth(float NewVal) {...}
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_SixParams(FDlkAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);
 /**
  * DlkAttributeSet
  * - Dlk�� ���������� Dlk���� ���� Attribute Set Class�̴�
  */
UCLASS()
class DEADLOCK_API UDlkAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UDlkAttributeSet();
};
