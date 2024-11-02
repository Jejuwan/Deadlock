// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "DlkPawnExtensionComponent.generated.h"

/**
 *
 */
class UDlkPawnData;
class UDlkPawnExtensionComponent;
class UDlkAbilitySystemComponent;
/**
 * �ʱ�ȭ ������ �����ϴ� ������Ʈ
 */
UCLASS()
class DEADLOCK_API UDlkPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
GENERATED_BODY()
	public:
		UDlkPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		/** FeatureName ���� */
		static const FName NAME_ActorFeatureName;

		/**
		* member methods
		*/
		/** Returns the pawn extension component if one exists on the specified actor. */
		UFUNCTION(BlueprintPure, Category = "Dlk|Pawn")
		static UDlkPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDlkPawnExtensionComponent>() : nullptr); }
		template <class T>
		const T* GetPawnData() const { return Cast<T>(PawnData); }
		void SetPawnData(const UDlkPawnData* InPawnData);
		void SetupPlayerInputComponent();
		UDlkAbilitySystemComponent* GetDlkAbilitySystemComponent() const { return AbilitySystemComponent; }
		/** AbilitySystemComponent�� AvatorActor ��� �ʱ�ȭ/���� ȣ�� */
		void InitializeAbilitySystem(UDlkAbilitySystemComponent* InASC, AActor* InOwnerActor);
		void UninitializeAbilitySystem();

		/** OnAbilitySystem[Initialized|Uninitialized] Delegate�� �߰�: */
		void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
		void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
		/**
		* UPawnComponent interfaces
		 */
		virtual void OnRegister() final;
		virtual void BeginPlay() final;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

		/**
		 * IGameFrameworkInitStateInterface
		 */
		virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
		virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
		virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
		virtual void CheckDefaultInitialization() final;

		/**
		 * Pawn�� ������ �����͸� ĳ��
		 */
		UPROPERTY(EditInstanceOnly, Category = "Dlk|Pawn")
		TObjectPtr<const UDlkPawnData> PawnData;

		/** AbilitySystemComponent ĳ�� */
		UPROPERTY()
		TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;

		/** ASC Init�� Uninit�� Delegate �߰� */
		FSimpleMulticastDelegate OnAbilitySystemInitialized;
		FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};
