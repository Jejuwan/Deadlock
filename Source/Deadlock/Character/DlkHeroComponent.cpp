// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "DlkPawnExtensionComponent.h"
#include "Deadlock/DlkLogChannels.h"
#include "Deadlock/DlkGameplayTags.h"
#include "Deadlock/Player/DlkPlayerState.h"
#include "Deadlock/Character/DlkPawnData.h"
#include "Deadlock/Camera/DlkCameraComponent.h"

/** FeatureName ����: static member variable �ʱ�ȭ */
const FName UDlkHeroComponent::NAME_ActorFeatureName("Hero");

UDlkHeroComponent::UDlkHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UDlkHeroComponent::OnRegister()
{
	Super::OnRegister();

	// �ùٸ� Actor�� ��ϵǾ����� Ȯ��:
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

	// PawnExtensionComponent�� ���ؼ� (PawnExtension Feature) OnActorInitStateChanged() �����ϵ��� (Observing)
	BindOnActorInitStateChanged(UDlkPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// InitState_Spawned�� �ʱ�ȭ
	ensure(TryToChangeInitState(FDlkGameplayTags::Get().InitState_Spawned));

	// ForceUpdate ����
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
		// HakPawnExtensionComponent�� DataInitialized ���� ��ȭ ���� ��, HakHeroComponent�� DataInitialized ���·� ����
		// - CanChangeInitState Ȯ��
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

	// Spawned �ʱ�ȭ
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
		// PawnExtensionComponent�� DataInitialized�� ������ ��ٸ� (== ��� Feature Component�� DataAvailable�� ����)
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

	// DataAvailable -> DataInitialized �ܰ�
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ADlkPlayerState* DlkPS = GetPlayerState<ADlkPlayerState>();
		if (!ensure(Pawn && DlkPS))
		{
			return;
		}

		// Input�� Camera�� ���� �ڵ鸵... (TODO)

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UDlkPawnData* PawnData = nullptr;
		if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UDlkPawnData>();
		}

		if (bIsLocallyControlled && PawnData)
		{
			// ���� HakCharacter�� Attach�� CameraComponent�� ã��
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
	// �ռ� BindOnActorInitStateChanged���� ���ҵ��� Hero Feature�� Pawn Extension Feature�� ���ӵǾ� �����Ƿ�, CheckDefaultInitializationForImplementers ȣ������ ����:

	// ContinueInitStateChain�� �ռ� PawnExtComponent�� ����
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