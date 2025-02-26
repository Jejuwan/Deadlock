// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkBurnExecution.h"
#include "Deadlock/AbilitySystem/Attributes/DlkCombatSet.h"
#include "Deadlock/AbilitySystem/Attributes/DlkBurntSet.h"

/**
 * 해당 Struct를 사용하여, FGameplayEffectAttributeCaptureDefinition 인스턴스화하여 전달한다
 */
struct FBurnStatics
{
	/** AttributeSet의 어떤 Attribute을 Capture할 것인지와 어떻게 Capture할지 정의를 담고 있다 (한번 보고 오자) */
	FGameplayEffectAttributeCaptureDefinition BaseBurnDef;

	FBurnStatics()
	{
		BaseBurnDef = FGameplayEffectAttributeCaptureDefinition(UDlkCombatSet::GetBaseBurnAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FBurnStatics& BurnStatics()
{
	// 계속 FBurnStatics를 생성하는 것은 부하이니, 한번만 생성하고 재사용한다
	static FBurnStatics Statics;
	return Statics;
}

UDlkBurnExecution::UDlkBurnExecution() : Super()
{
	// Source로 (입력값) Attribute를 캡쳐를 정의하자
	// - CombatSet::BaseBurn을 통해 Burning 값을 정의하고 최종 Execute할 때, 해당 값을 가져와서 Burnth에 Burning을 적용한다
	RelevantAttributesToCapture.Add(BurnStatics().BaseBurnDef);
}

void UDlkBurnExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// GameplayEffectSpec은 GameplayEffect의 핸들로 생각하면 된다
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float BaseBurn = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		// 해당 함수 호출을 통해 DlkCombatSet의 BaseBurn 값을 가져온다 (혹은 뭔가 Modifier 에 누적되어 있었다면, 최종 계산 결과가 나온다)
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BurnStatics().BaseBurnDef, EvaluateParameters, BaseBurn);
	}

	// RelevantAttributesCapture을 통해 최종 계산된 BaseBurn을 0.0이하가 되지 않도록 한다 (Burning이니깐!)
	const float BurningDone = FMath::Max(0.0f, BaseBurn);
	if (BurningDone > 0.0f)
	{
		// GameplayEffectCalculation 이후, Modifier로서, 추가한다:
		// - 해당 Modifier는 CombatSet에서 가져온 BaseBurn을 활용하여, BurnthSet의 Burning에 추가해준다
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDlkBurntSet::GetIncreaseAttribute(), EGameplayModOp::Additive, BurningDone));
	}
}