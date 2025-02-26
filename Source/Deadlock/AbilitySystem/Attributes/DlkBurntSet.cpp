// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkBurntSet.h"
#include "GameplayEffectExtension.h"
#include "DlkAttributeSet.h"
#include "Deadlock/DlkLogChannels.h"

UDlkBurntSet::UDlkBurntSet() : Super(), Degree(50), MaxDegree(100)
{
	bOutOfDegree = false;
	MaxDegreeBeforeAttributeChange = 0.0f;
	DegreeBeforeAttributeChange = 0.0f;
}

void UDlkBurntSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION_NOTIFY(ULyraHealthSet, Health, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(ULyraHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);

}
void UDlkBurntSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// HealthAttribute는 [0,GetMaxHealth]로 설정
	if (Attribute == GetDegreeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxDegree());
	}
	// MaxHealthAttribute는 [1.0, inf]로 설정:
	// - 즉, MaxHealth는 1미만이 될 수 없음!
	else if (Attribute == GetMaxDegreeAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UDlkBurntSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UDlkBurntSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

bool UDlkBurntSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	DegreeBeforeAttributeChange = GetDegree();
	MaxDegreeBeforeAttributeChange = GetMaxDegree();

	return true;
}

void UDlkBurntSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	float MinimumHealth = 0.0f;
	if (Data.EvaluatedData.Attribute == GetIncreaseAttribute())
	{
		SetDegree(FMath::Clamp(GetDegree() + GetIncrease(), MinimumHealth, GetMaxDegree()));
		SetIncrease(0.0f);
	}
	if (Data.EvaluatedData.Attribute == GetDecreaseAttribute())
	{
		SetDegree(FMath::Clamp(GetDegree() - GetDecrease(), MinimumHealth, GetMaxDegree()));
		SetDecrease(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetDegreeAttribute())
	{
		SetDegree(FMath::Clamp(GetDegree(), MinimumHealth, GetMaxDegree()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxDegreeAttribute())
	{
	}

	if (GetDegree() != DegreeBeforeAttributeChange)
	{
		OnDegreeChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, DegreeBeforeAttributeChange, GetDegree());
	}

	if ((GetDegree() >= GetMaxDegree()))
	{
		OnFullOfDegree.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, DegreeBeforeAttributeChange, GetDegree());
	}

	if ((GetDegree() <= 0.0f) && !bOutOfDegree)
	{
		OnOutOfDegree.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, DegreeBeforeAttributeChange, GetDegree());
	}

	bOutOfDegree = (GetDegree() <= 0.0f);
}

void UDlkBurntSet::OnRep_Degree(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDlkBurntSet, Degree, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentDegree = GetDegree();
	const float EstimatedMagnitude = CurrentDegree - OldValue.GetCurrentValue();

	OnDegreeChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentDegree);

	if (CurrentDegree >= GetMaxDegree())
	{
		OnFullOfDegree.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentDegree);
	}

	if (!bOutOfDegree && CurrentDegree <= 0.0f)
	{
		OnOutOfDegree.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentDegree);
	}

	bOutOfDegree = (CurrentDegree <= 0.0f);
}
