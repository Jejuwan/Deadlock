#pragma once

#include "Components/GameFrameworkComponent.h"
#include "Delegates/Delegate.h"
#include "DlkBurntComponent.generated.h"

/** forward declarations */
class UDlkAbilitySystemComponent;
class UDlkBurntSet;
class UDlkBurntComponent;
class AActor;
struct FOnAttributeChangeData;
struct FGameplayEffectSpec;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDlkDegree_BurntEvent, AActor*, OwningActor);
/** Degree 변화 콜백을 위한 델레게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDlkDegree_AttributeChanged, UDlkBurntComponent*, DegreeComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UENUM(BlueprintType)
enum class EDlkBurntState : uint8
{
	NotBurn = 0,
	BurnStarted,
	BurnFinished
};

/**
 * Character(Pawn)에 대해 체력관련 처리를 담당하는 Component이다
 * - 참고로 해당 클래스는 Blueprintable이다:
 * - 이는 멤버변수인 Delegate를 UI에서 바인딩하기 위함이다 (자세한건 클론하면서 알아보자)
 */
UCLASS(Blueprintable)
class UDlkBurntComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
public:
	UDlkBurntComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * BP 지원 메서드:
	 */

	/** Actor(보통 ACharacter/APawn)의 DegreeComponent를 반환 */
	UFUNCTION(BlueprintPure, Category="Dlk|Degree")
	static UDlkBurntComponent* FindBurntComponent(const AActor* Actor);

	/** 아래의 UFUNCTION은 DegreeSet의 Attribute에 접근하기 위한 BP Accessor 함수들 */
	UFUNCTION(BlueprintCallable, Category="Dlk|Degree")
	float GetDegree() const;

	UFUNCTION(BlueprintCallable, Category="Dlk|Degree")
	float GetMaxDegree() const;

	UFUNCTION(BlueprintCallable, Category="Dlk|Degree")
	float GetDegreeNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Degree")
	EDlkBurntState GetBurntState() const { return BurntState; }


	virtual void StartBurn();

	virtual void FinishBurn();


	/** ASC와 DegreeSet 초기화 */
	void InitializeWithAbilitySystem(UDlkAbilitySystemComponent* InASC);
	void UninitializeWithAbilitySystem();

	UPROPERTY()
	TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UDlkBurntSet> BurntSet;

	UPROPERTY(BlueprintAssignable)
	FDlkDegree_AttributeChanged OnDegreeChanged;

	UPROPERTY(BlueprintAssignable)
	FDlkDegree_AttributeChanged OnMaxDegreeChanged;

	UPROPERTY(BlueprintAssignable)
	FDlkDegree_BurntEvent OnBurntStarted;

	UPROPERTY(BlueprintAssignable)
	FDlkDegree_BurntEvent OnBurntFinished;


	virtual void HandleDegreeChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxDegreeChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleFullOfDegree(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfDegree(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:
	EDlkBurntState BurntState;
};