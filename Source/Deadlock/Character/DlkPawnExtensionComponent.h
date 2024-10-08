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
/**
 * 초기화 전반을 조정하는 컴포넌트
 */
UCLASS()
class DEADLOCK_API UDlkPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
GENERATED_BODY()
	public:
		UDlkPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

		/** FeatureName 정의 */
		static const FName NAME_ActorFeatureName;

		/**
		* member methods
		*/
		static UDlkPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDlkPawnExtensionComponent>() : nullptr); }
		template <class T>
		const T* GetPawnData() const { return Cast<T>(PawnData); }
		void SetPawnData(const UDlkPawnData* InPawnData);
		void SetupPlayerInputComponent();

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
		 * Pawn을 생성한 데이터를 캐싱
		 */
		UPROPERTY(EditInstanceOnly, Category = "Dlk|Pawn")
		TObjectPtr<const UDlkPawnData> PawnData;
};
