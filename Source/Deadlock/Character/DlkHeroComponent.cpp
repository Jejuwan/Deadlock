// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "DlkPawnExtensionComponent.h"
#include "Deadlock/DlkLogChannels.h"
#include "Deadlock/DlkGameplayTags.h"
#include "Deadlock/Player/DlkPlayerState.h"
#include "Deadlock/Character/DlkPawnData.h"
#include "Deadlock/Camera/DlkCameraComponent.h"

/** FeatureName 정의: static member variable 초기화 */
const FName UDlkHeroComponent::NAME_ActorFeatureName("Hero");

UDlkHeroComponent::UDlkHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UDlkHeroComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 Actor에 등록되었는지 확인:
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogDlk, Error, TEXT("this component has been added to a BP whose base class is not a Pawn!"));
			return;
		}
	}

	RegisterInitStateFeature();
}

void UDlkHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// PawnExtensionComponent에 대해서 (PawnExtension Feature) OnActorInitStateChanged() 관찰하도록 (Observing)
	BindOnActorInitStateChanged(UDlkPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FDlkGameplayTags::Get().InitState_Spawned));

	// ForceUpdate 진행
	CheckDefaultInitialization();
}

void UDlkHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UDlkHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FDlkGameplayTags& InitTags = FDlkGameplayTags::Get();

	if (Params.FeatureName == UDlkPawnExtensionComponent::NAME_ActorFeatureName)
	{
		// HakPawnExtensionComponent의 DataInitialized 상태 변화 관찰 후, HakHeroComponent도 DataInitialized 상태로 변경
		// - CanChangeInitState 확인
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UDlkHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FDlkGameplayTags& InitTags = FDlkGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	ADlkPlayerState* DlkPS = GetPlayerState<ADlkPlayerState>();

	// Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!DlkPS)
		{
			return false;
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// PawnExtensionComponent가 DataInitialized될 때까지 기다림 (== 모든 Feature Component가 DataAvailable인 상태)
		return DlkPS && Manager->HasFeatureReachedInitState(Pawn, UDlkPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}
PRAGMA_DISABLE_OPTIMIZATION
void UDlkHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FDlkGameplayTags& InitTags = FDlkGameplayTags::Get();

	// DataAvailable -> DataInitialized 단계
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ADlkPlayerState* DlkPS = GetPlayerState<ADlkPlayerState>();
		if (!ensure(Pawn && DlkPS))
		{
			return;
		}

		// Input과 Camera에 대한 핸들링... (TODO)

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UDlkPawnData* PawnData = nullptr;
		if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UDlkPawnData>();
		}

		if (bIsLocallyControlled && PawnData)
		{
			// 현재 HakCharacter에 Attach된 CameraComponent를 찾음
			if (UDlkCameraComponent* CameraComponent = UDlkCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION

void UDlkHeroComponent::CheckDefaultInitialization()
{
	// 앞서 BindOnActorInitStateChanged에서 보았듯이 Hero Feature는 Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers 호출하지 않음:

	// ContinueInitStateChain은 앞서 PawnExtComponent와 같음
	const FDlkGameplayTags& InitTags = FDlkGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

PRAGMA_DISABLE_OPTIMIZATION
TSubclassOf<UDlkCameraMode> UDlkHeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UDlkPawnData* PawnData = PawnExtComp->GetPawnData<UDlkPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}
PRAGMA_ENABLE_OPTIMIZATION