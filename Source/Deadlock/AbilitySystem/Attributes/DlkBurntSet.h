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
	* 앞서 DlkAttributeSet에서 정의했던, ATTRIBUTE_ACCESSORS를 통해, 아래 정의함 멤벼변수와 똑같이 이름을 설정한다
	* - ATTRIBUTE_ACCESSORS의 Macro의 정의부분을 한번 실펴보자	
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
	* Attribute의 값을 ClampAttribute()를 활용하여, 값의 범위를 유지시켜주기 위해
	* PreAttributeBaseChange와 PreAttributeChange 오버라이드
	*/
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	* GameplayEffect가 HealthSet의 Attribute를 수정하기 전에 불리는 콜백함수이다:
	* - 이는 AttributeSet의 주석에도 잘 나와있듯이, Healing이 업데이트되면, Health를 Healing을 적용하여 업데이트 가능하다
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
