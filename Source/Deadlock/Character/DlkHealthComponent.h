#pragma once

#include "Components/GameFrameworkComponent.h"
#include "Delegates/Delegate.h"
#include "DlkHealthComponent.generated.h"

/** forward declarations */
class UDlkAbilitySystemComponent;
class UDlkHealthSet;
class UDlkHealthComponent;
class AActor;
struct FOnAttributeChangeData;
struct FGameplayEffectSpec;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDlkHealth_DeathEvent, AActor*, OwningActor);
/** Health 변화 콜백을 위한 델레게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDlkHealth_AttributeChanged, UDlkHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UENUM(BlueprintType)
enum class EDlkDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

/**
 * Character(Pawn)에 대해 체력관련 처리를 담당하는 Component이다
 * - 참고로 해당 클래스는 Blueprintable이다:
 * - 이는 멤버변수인 Delegate를 UI에서 바인딩하기 위함이다 (자세한건 클론하면서 알아보자)
 */
UCLASS(Blueprintable)
class UDlkHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
public:
	UDlkHealthComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * BP 지원 메서드:
	 */

	/** Actor(보통 ACharacter/APawn)의 HealthComponent를 반환 */
	UFUNCTION(BlueprintPure, Category="Dlk|Health")
	static UDlkHealthComponent* FindHealthComponent(const AActor* Actor);

	/** 아래의 UFUNCTION은 HealthSet의 Attribute에 접근하기 위한 BP Accessor 함수들 */
	UFUNCTION(BlueprintCallable, Category="Dlk|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category="Dlk|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category="Dlk|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	EDlkDeathState GetDeathState() const { return DeathState; }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	/** ASC와 HealthSet 초기화 */
	void InitializeWithAbilitySystem(UDlkAbilitySystemComponent* InASC);
	void UninitializeWithAbilitySystem();

	/** ASC를 통해, HealthSet의 HealthAttribute 변경이 있을때 호출하는 메서드 (내부적으로 OnHealthChanged 호출) */
	//void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);

	/** HealthSet을 접근하기 위한 AbilitySystemComponent */
	UPROPERTY()
	TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;

	/** 캐싱된 HealthSet 레퍼런스 */
	UPROPERTY()
	TObjectPtr<const UDlkHealthSet> HealthSet;

	/** health 변화에 따른 Delegate(Multicast) */
	UPROPERTY(BlueprintAssignable)
	FDlkHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FDlkHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FDlkHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FDlkHealth_DeathEvent OnDeathFinished;

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:
	EDlkDeathState DeathState;
};