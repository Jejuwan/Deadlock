// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DlkHealthSet.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkHealthSet : public UDlkAttributeSet
{
	GENERATED_BODY()
public:
	UDlkHealthSet();

	/**
	* �ռ� DlkAttributeSet���� �����ߴ�, ATTRIBUTE_ACCESSORS�� ����, �Ʒ� ������ �⺭������ �Ȱ��� �̸��� �����Ѵ�
	* - ATTRIBUTE_ACCESSORS�� Macro�� ���Ǻκ��� �ѹ� ���캸��	
	*/
	ATTRIBUTE_ACCESSORS(UDlkHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UDlkHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UDlkHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UDlkHealthSet, Damage);

	/**
	* Attribute�� ���� ClampAttribute()�� Ȱ���Ͽ�, ���� ������ ���������ֱ� ����
	* PreAttributeBaseChange�� PreAttributeChange �������̵�
	*/
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	* GameplayEffect�� HealthSet�� Attribute�� �����ϱ� ���� �Ҹ��� �ݹ��Լ��̴�:
	* - �̴� AttributeSet�� �ּ����� �� �����ֵ���, Healing�� ������Ʈ�Ǹ�, Health�� Healing�� �����Ͽ� ������Ʈ �����ϴ�
	*/
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** ���� ü�� */
	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData Health;

	/** ü�� �ִ�ġ */
	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData MaxHealth;

	/** ü�� ȸ��ġ */
	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData Healing;

	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData Damage;
};
