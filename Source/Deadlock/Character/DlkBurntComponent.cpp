#include "DlkBurntComponent.h"
#include "Deadlock/DlkLogChannels.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Deadlock/DlkGameplayTags.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "Deadlock/AbilitySystem/Attributes/DlkBurntSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkBurntComponent)

UDlkBurntComponent::UDlkBurntComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeWithAbilitySystem으로 ASC가 초기화되기 전까지 DegreeSet과 ASC는 null이다:
	AbilitySystemComponent = nullptr;
	BurntSet = nullptr;
}

void UDlkBurntComponent::InitializeWithAbilitySystem(UDlkAbilitySystemComponent* InASC)
{
	// AActor는 DlkCharacter를 상속받고 있는 클래스일 것이다
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogDlk, Error, TEXT("DlkBurntComponent: Degree component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	// ASC 캐싱
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogDlk, Error, TEXT("DlkBurntComponent: Cannot initialize Degree component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	BurntSet = AbilitySystemComponent->GetSet<UDlkBurntSet>();

	if (!BurntSet)
	{
		UE_LOG(LogDlk, Error, TEXT("DlkBurntComponent: Cannot initialize Degree component for owner [%s] with NULL Degree set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// DegreeSet의 DegreeAttribute의 업데이트가 일어날때마다 호출할 콜백으로 멤버메서드 HandleDegreeChanged를 등록하자:
	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDlkDegreeSet::GetDegreeAttribute()).AddUObject(this, &ThisClass::HandleDegreeChanged);


	// Register to listen for attribute changes.
	BurntSet->OnDegreeChanged.AddUObject(this, &ThisClass::HandleDegreeChanged);
	BurntSet->OnFullOfDegree.AddUObject(this, &ThisClass::HandleFullOfDegree);
	BurntSet->OnOutOfDegree.AddUObject(this, &ThisClass::HandleOutOfDegree);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	AbilitySystemComponent->SetNumericAttributeBase(UDlkBurntSet::GetDegreeAttribute(), 0);


	// 초기화 한번 해줬으니깐 Broadcast 해주자
	OnDegreeChanged.Broadcast(this, BurntSet->GetDegree(), BurntSet->GetDegree(), nullptr);
	OnMaxDegreeChanged.Broadcast(this, BurntSet->GetDegree(), BurntSet->GetDegree(), nullptr);
}

void UDlkBurntComponent::UninitializeWithAbilitySystem()
{
	if (BurntSet)
	{
		BurntSet->OnDegreeChanged.RemoveAll(this);
		BurntSet->OnFullOfDegree.RemoveAll(this);
		BurntSet->OnOutOfDegree.RemoveAll(this);
	}

	BurntSet = nullptr;
	AbilitySystemComponent = nullptr;

}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	// GameEffectModifier에 Data가 있을 경우만 호출되는가보다 (사실 우리는 크게 관심없음)
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}
	return nullptr;
}

void UDlkBurntComponent::HandleDegreeChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnDegreeChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UDlkBurntComponent::HandleMaxDegreeChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	//OnMaxDegreeChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UDlkBurntComponent::HandleFullOfDegree(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
}

void UDlkBurntComponent::HandleOutOfDegree(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
		if (AbilitySystemComponent && DamageEffectSpec)
		{
			{
			/*	FGameplayEventData Payload;
				Payload.EventTag = DlkGameplayTags::GameplayEvent_Death;
				Payload.Instigator = DamageInstigator;
				Payload.Target = AbilitySystemComponent->GetAvatarActor();
				Payload.OptionalObject = DamageEffectSpec->Def;
				Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
				Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
				Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
				Payload.EventMagnitude = DamageMagnitude;

				FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
				AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);*/
			}

		}

#endif // #if WITH_SERVER_CODE
}

UDlkBurntComponent* UDlkBurntComponent::FindBurntComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UDlkBurntComponent* DegreeComponent = Actor->FindComponentByClass<UDlkBurntComponent>();
	return DegreeComponent;
}

float UDlkBurntComponent::GetDegree() const
{
	return (BurntSet ? BurntSet->GetDegree() : 0.0f);
}

float UDlkBurntComponent::GetMaxDegree() const
{
	return (BurntSet ? BurntSet->GetMaxDegree() : 0.0f);
}

float UDlkBurntComponent::GetDegreeNormalized() const
{
	if (BurntSet)
	{
		const float Degree = BurntSet->GetDegree();
		const float MaxDegree = BurntSet->GetMaxDegree();
		return ((MaxDegree > 0.0f) ? (Degree / MaxDegree) : 0.0f);
	}
	return 0.0f;
}

void UDlkBurntComponent::StartBurn()
{
	//if (DeathState != EDlkDeathState::NotDead)
	//{
	//	return;
	//}

	//DeathState = EDlkDeathState::DeathStarted;

	//if (AbilitySystemComponent)
	//{
	//	//AbilitySystemComponent->SetLooseGameplayTagCount(DEADLOCKplayTags::Status_Death_Dying, 1);
	//}

	//AActor* Owner = GetOwner();
	//check(Owner);

	//OnDeathStarted.Broadcast(Owner);

	////Owner->ForceNetUpdate();
}

void UDlkBurntComponent::FinishBurn()
{
	//if (DeathState != EDlkDeathState::DeathStarted)
	//{
	//	return;
	//}

	//DeathState = EDlkDeathState::DeathFinished;

	//if (AbilitySystemComponent)
	//{
	//	//AbilitySystemComponent->SetLooseGameplayTagCount(DEADLOCKplayTags::Status_Death_Dead, 1);
	//}

	//AActor* Owner = GetOwner();
	//check(Owner);

	//OnDeathFinished.Broadcast(Owner);

	////Owner->ForceNetUpdate();
}

