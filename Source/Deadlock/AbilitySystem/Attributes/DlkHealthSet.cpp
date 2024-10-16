// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkHealthSet.h"
#include "GameplayEffectExtension.h"

UDlkHealthSet::UDlkHealthSet() : Super(), Health(50), MaxHealth(100)
{
}

void UDlkHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// HealthAttribute�� [0,GetMaxHealth]�� ����
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	// MaxHealthAttribute�� [1.0, inf]�� ����:
	// - ��, MaxHealth�� 1�̸��� �� �� ����!
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UDlkHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UDlkHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

bool UDlkHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UDlkHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;

	// *** Healing�� ������Ʈ �� ���, Healing�� Health�� �����ϰ�, Healing�� �ʱ�ȭ���ش�
	if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	// Health ������Ʈ�� ���, [0,MaxHealth]�� ���߾�����
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
}