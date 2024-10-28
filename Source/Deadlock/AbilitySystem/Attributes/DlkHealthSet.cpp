// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkHealthSet.h"
#include "GameplayEffectExtension.h"

UDlkHealthSet::UDlkHealthSet() : Super(), Health(50), MaxHealth(100)
{
}

void UDlkHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// HealthAttribute는 [0,GetMaxHealth]로 설정
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	// MaxHealthAttribute는 [1.0, inf]로 설정:
	// - 즉, MaxHealth는 1미만이 될 수 없음!
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
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		//if (Data.EvaluatedData.Magnitude > 0.0f)
		//{
		//	FLyraVerbMessage Message;
		//	Message.Verb = TAG_Lyra_Damage_Message;
		//	Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
		//	Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
		//	Message.Target = GetOwningActor();
		//	Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
		//	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
		//	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
		//	Message.Magnitude = Data.EvaluatedData.Magnitude;

		//	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		//	MessageSystem.BroadcastMessage(Message.Verb, Message);
		//}

		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	// *** Healing이 업데이트 될 경우, Healing을 Health에 적용하고, Healing을 초기화해준다
	if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	// Health 업데이트의 경우, [0,MaxHealth]로 맞추어주자
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
}