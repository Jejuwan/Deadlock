// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DlkBurntSet.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkBurntSet : public UDlkAttributeSet
{
	GENERATED_BODY()
public:
	UDlkBurntSet();

	/**
	* �ռ� DlkAttributeSet���� �����ߴ�, ATTRIBUTE_ACCESSORS�� ����, �Ʒ� ������ �⺭������ �Ȱ��� �̸��� �����Ѵ�
	* - ATTRIBUTE_ACCESSORS�� Macro�� ���Ǻκ��� �ѹ� ���캸��	
	*/
	ATTRIBUTE_ACCESSORS(UDlkBurntSet, Degree);
	ATTRIBUTE_ACCESSORS(UDlkBurntSet, MaxDegree);
	ATTRIBUTE_ACCESSORS(UDlkBurntSet, Decrease);
	ATTRIBUTE_ACCESSORS(UDlkBurntSet, Increase);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FDlkAttributeEvent OnDegreeChanged;

	mutable FDlkAttributeEvent OnFullOfDegree;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FDlkAttributeEvent OnOutOfDegree;

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

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Degree, Category = "Dlk|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Degree;

	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData MaxDegree;

	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData Decrease;

	UPROPERTY(BlueprintReadOnly, Category = "Dlk|Health")
	FGameplayAttributeData Increase;

	bool bOutOfDegree;

	float MaxDegreeBeforeAttributeChange;
	float DegreeBeforeAttributeChange;

protected:
	UFUNCTION()
	void OnRep_Degree(const FGameplayAttributeData& OldValue);
};
