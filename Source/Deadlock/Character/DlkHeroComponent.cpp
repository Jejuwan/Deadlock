// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "DlkPawnExtensionComponent.h"
#include "PlayerMappableInputConfig.h"
#include "Deadlock/Input/DlkMappableConfigPair.h"
#include "Deadlock/Input/DlkInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Deadlock/DlkLogChannels.h"
#include "Deadlock/DlkGameplayTags.h"
#include "Deadlock/Player/DlkPlayerState.h"
#include "Deadlock/Player/DlkPlayerController.h"
#include "Deadlock/Character/DlkPawnData.h"
#include "Deadlock/Camera/DlkCameraComponent.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "GameFramework/Character.h"

/** FeatureName 정의: static member variable 초기화 */
const FName UDlkHeroComponent::NAME_ActorFeatureName("Hero");

/** InputConfig의 GameFeatureAction 활성화 ExtensioEvent 이름 */
const FName UDlkHeroComponent::NAME_BindInputsNow("BindInputsNow");

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
	ensure(TryToChangeInitState(DlkGameplayTags::InitState_Spawned));

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
	if (Params.FeatureName == UDlkPawnExtensionComponent::NAME_ActorFeatureName)
	{
		// DlkPawnExtensionComponent의 DataInitialized 상태 변화 관찰 후, DlkHeroComponent도 DataInitialized 상태로 변경
		// - CanChangeInitState 확인
		if (Params.FeatureState == DlkGameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UDlkHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	ADlkPlayerState* DlkPS = GetPlayerState<ADlkPlayerState>();

	// Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == DlkGameplayTags::InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == DlkGameplayTags::InitState_Spawned && DesiredState == DlkGameplayTags::InitState_DataAvailable)
	{
		if (!DlkPS)
		{
			return false;
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == DlkGameplayTags::InitState_DataAvailable && DesiredState == DlkGameplayTags::InitState_DataInitialized)
	{
		// PawnExtensionComponent가 DataInitialized될 때까지 기다림 (== 모든 Feature Component가 DataAvailable인 상태)
		return DlkPS && Manager->HasFeatureReachedInitState(Pawn, UDlkPawnExtensionComponent::NAME_ActorFeatureName, DlkGameplayTags::InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == DlkGameplayTags::InitState_DataInitialized && DesiredState == DlkGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}
PRAGMA_DISABLE_OPTIMIZATION
void UDlkHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// DataAvailable -> DataInitialized 단계
	if (CurrentState == DlkGameplayTags::InitState_DataAvailable && DesiredState == DlkGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ADlkPlayerState* DlkPS = GetPlayerState<ADlkPlayerState>();
		if (!ensure(Pawn && DlkPS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UDlkPawnData* PawnData = nullptr;
		if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UDlkPawnData>();

			// DataInitialized 단계까지 오면, Pawn이 Controller에 Possess되어 준비된 상태이다:
			// - InitAbilityActorInfo 호출로 AvatarActor 재설정이 필요하다
			PawnExtComp->InitializeAbilitySystem(DlkPS->GetDlkAbilitySystemComponent(), DlkPS);
		}

		// Input과 Camera에 대한 핸들링... (TODO)

		if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UDlkPawnData>();
		}

		if (bIsLocallyControlled && PawnData)
		{
			// 현재 DlkCharacter에 Attach된 CameraComponent를 찾음
			if (UDlkCameraComponent* CameraComponent = UDlkCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		if (ADlkPlayerController* DlkPC = GetController<ADlkPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION

void UDlkHeroComponent::CheckDefaultInitialization()
{
	// 앞서 BindOnActorInitStateChanged에서 보았듯이 Hero Feature는 Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers 호출하지 않음:

	// ContinueInitStateChain은 앞서 PawnExtComponent와 같음
	static const TArray<FGameplayTag> StateChain = { DlkGameplayTags::InitState_Spawned, DlkGameplayTags::InitState_DataAvailable, DlkGameplayTags::InitState_DataInitialized, DlkGameplayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

PRAGMA_DISABLE_OPTIMIZATION
TSubclassOf<UDlkCameraMode> UDlkHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

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

void UDlkHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// LocalPlayer를 가져오기 위해
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem 가져오기 위해
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// EnhancedInputLocalPlayerSubsystem에 MappingContext를 비워준다:
	Subsystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData -> InputConfig 존재 유무 판단:
	if (const UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UDlkPawnData* PawnData = PawnExtComp->GetPawnData<UDlkPawnData>())
		{
			if (const UDlkInputConfig* InputConfig = PawnData->InputConfig)
			{
				// HeroComponent 가지고 있는 Input Mapping Context를 순회하며, EnhancedInputLocalPlayerSubsystem에 추가한다
				for (const FDlkMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically)
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						// 내부적으로 Input Mapping Context를 추가한다:
						// - AddPlayerMappableConfig를 간단히 보는 것을 추천
						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
					}
				}

				UDlkInputComponent* DlkIC = CastChecked<UDlkInputComponent>(PlayerInputComponent);
				{
					// InputTag_Move와 InputTag_Look_Mouse에 대해 각각 Input_Move()와 Input_LookMouse() 멤버 함수에 바인딩시킨다:
		// - 바인딩한 이후, Input 이벤트에 따라 멤버 함수가 트리거된다
					{
						TArray<uint32> BindHandles;
						DlkIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
					}
					// InputTag_Move와 InputTag_Look_Mouse에 대해 각각 Input_Move()와 Input_LookMouse() 멤버 함수에 바인딩시킨다:
					// - 바인딩한 이후, Input 이벤트에 따라 멤버 함수가 트리거된다
					DlkIC->BindNativeAction(InputConfig, DlkGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					DlkIC->BindNativeAction(InputConfig, DlkGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
					DlkIC->BindNativeAction(InputConfig, DlkGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump, false);
				}
			}
		}
	}

	// GameFeatureAction_AddInputConfig의 HandlePawnExtension 콜백 함수 전달
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UDlkHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			// Left/Right -> X 값에 들어있음:
			// MovementDirection은 현재 카메라의 RightVector를 의미함 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovementInput 함수를 한번 보자:
			// - 내부적으로 MovementDirection * Value.X를 MovementComponent에 적용(더하기)해준다
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f) // 앞서 우리는 Forward 적용을 위해 swizzle input modifier를 사용했다~
		{
			// 앞서 Left/Right와 마찬가지로 Forward/Backward를 적용한다
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UDlkHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// X에는 Yaw 값이 있음:
		// - Camera에 대해 Yaw 적용
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y에는 Pitch 값!
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}

void UDlkHeroComponent::Input_Jump(const FInputActionValue& InputActionValue)
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			OwningCharacter->Jump();
		}
	}
}

void UDlkHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UDlkAbilitySystemComponent* DlkASC = PawnExtComp->GetDlkAbilitySystemComponent())
			{
				DlkASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UDlkHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UDlkAbilitySystemComponent* DlkASC = PawnExtComp->GetDlkAbilitySystemComponent())
			{
				DlkASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UDlkHeroComponent::SetAbilityCameraMode(TSubclassOf<UDlkCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UDlkHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}
