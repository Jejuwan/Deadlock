#include "DlkHealthComponent.h"
#include "Deadlock/DlkLogChannels.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Deadlock/DlkGameplayTags.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "Deadlock/AbilitySystem/Attributes/DlkHealthSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkHealthComponent)

UDlkHealthComponent::UDlkHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthComponent는 PlayerState의 HealthSet과 Character(Pawn)간 Bridge 역활이라고 생각하면 된다:
	// - 따로 로직이 업데이트 될 필요가 없는 이벤트 기반으로 동작하는 컴포넌트로 이해하면 된다
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeWithAbilitySystem으로 ASC가 초기화되기 전까지 HealthSet과 ASC는 null이다:
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UDlkHealthComponent::InitializeWithAbilitySystem(UDlkAbilitySystemComponent* InASC)
{
	// AActor는 DlkCharacter를 상속받고 있는 클래스일 것이다
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogDlk, Error, TEXT("DlkHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	// ASC 캐싱
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogDlk, Error, TEXT("DlkHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	// AbilitySystemComponent::GetSet은 SpawnedAttributes에서 가져온다:
	// - 근데 PlayerState에서 Subobject로 생성하고 따로 ASC에 등록한적이 없는데 어떻게 등록되어있을까?
	//   - AbilitySystemComponent::InitializeComponent()에서 GetObjectsWithOuter로 SpawnedAttributes에 추가된다:
	//   - 잘 생각해보자 HealthSet은 PlayerState에 Subobject로 있고, ASC 또한 PlayerState에 있다:
	//     -> 이는 ASC에서 GetObjectsWithOuter로 HealthSet이 접근된다!!!
	// - 한번 AbilitySystemComponent::InitializeComponent()을 보자
	HealthSet = AbilitySystemComponent->GetSet<UDlkHealthSet>();

	if (!HealthSet)
	{
		UE_LOG(LogDlk, Error, TEXT("DlkHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// HealthSet의 HealthAttribute의 업데이트가 일어날때마다 호출할 콜백으로 멤버메서드 HandleHealthChanged를 등록하자:
	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDlkHealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);


	// Register to listen for attribute changes.
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	AbilitySystemComponent->SetNumericAttributeBase(UDlkHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());


	// 초기화 한번 해줬으니깐 Broadcast 해주자
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UDlkHealthComponent::UninitializeWithAbilitySystem()
{
	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
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

void UDlkHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UDlkHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	//OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UDlkHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
		if (AbilitySystemComponent && DamageEffectSpec)
		{
			// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
			{
				FGameplayEventData Payload;
				Payload.EventTag = DlkGameplayTags::GameplayEvent_Death;
				Payload.Instigator = DamageInstigator;
				Payload.Target = AbilitySystemComponent->GetAvatarActor();
				Payload.OptionalObject = DamageEffectSpec->Def;
				Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
				Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
				Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
				Payload.EventMagnitude = DamageMagnitude;

				FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
				AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
			}

			// Send a standardized verb message that other systems can observe
			//{
			//	FLyraVerbMessage Message;
			//	Message.Verb = TAG_Lyra_Elimination_Message;
			//	Message.Instigator = DamageInstigator;
			//	Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			//	Message.Target = ULyraVerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
			//	Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			//	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...

			//	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			//	MessageSystem.BroadcastMessage(Message.Verb, Message);
			//}

			//@TODO: assist messages (could compute from damage dealt elsewhere)?
		}

#endif // #if WITH_SERVER_CODE
}

UDlkHealthComponent* UDlkHealthComponent::FindHealthComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UDlkHealthComponent* HealthComponent = Actor->FindComponentByClass<UDlkHealthComponent>();
	return HealthComponent;
}

float UDlkHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UDlkHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UDlkHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}
	return 0.0f;
}

void UDlkHealthComponent::StartDeath()
{
	if (DeathState != EDlkDeathState::NotDead)
	{
		return;
	}

	DeathState = EDlkDeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		//AbilitySystemComponent->SetLooseGameplayTagCount(DEADLOCKplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	//Owner->ForceNetUpdate();
}

void UDlkHealthComponent::FinishDeath()
{
	if (DeathState != EDlkDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EDlkDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		//AbilitySystemComponent->SetLooseGameplayTagCount(DEADLOCKplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	//Owner->ForceNetUpdate();
}

void UDlkHealthComponent::ExecuteStartDeath()
{
	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);
}

void UDlkHealthComponent::ExecuteFinishDeath()
{
	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);
}
