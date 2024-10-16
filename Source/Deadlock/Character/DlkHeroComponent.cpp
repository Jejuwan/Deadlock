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

/** FeatureName ����: static member variable �ʱ�ȭ */
const FName UDlkHeroComponent::NAME_ActorFeatureName("Hero");

/** InputConfig�� GameFeatureAction Ȱ��ȭ ExtensioEvent �̸� */
const FName UDlkHeroComponent::NAME_BindInputsNow("BindInputsNow");

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
		// DlkPawnExtensionComponent�� DataInitialized ���� ��ȭ ���� ��, DlkHeroComponent�� DataInitialized ���·� ����
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

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UDlkPawnData* PawnData = nullptr;
		if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UDlkPawnData>();

			// DataInitialized �ܰ���� ����, Pawn�� Controller�� Possess�Ǿ� �غ�� �����̴�:
			// - InitAbilityActorInfo ȣ��� AvatarActor �缳���� �ʿ��ϴ�
			PawnExtComp->InitializeAbilitySystem(DlkPS->GetDlkAbilitySystemComponent(), DlkPS);
		}

		// Input�� Camera�� ���� �ڵ鸵... (TODO)

		if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UDlkPawnData>();
		}

		if (bIsLocallyControlled && PawnData)
		{
			// ���� DlkCharacter�� Attach�� CameraComponent�� ã��
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

void UDlkHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// LocalPlayer�� �������� ����
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem �������� ����
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// EnhancedInputLocalPlayerSubsystem�� MappingContext�� ����ش�:
	Subsystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData -> InputConfig ���� ���� �Ǵ�:
	if (const UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UDlkPawnData* PawnData = PawnExtComp->GetPawnData<UDlkPawnData>())
		{
			if (const UDlkInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FDlkGameplayTags& GameplayTags = FDlkGameplayTags::Get();

				// HeroComponent ������ �ִ� Input Mapping Context�� ��ȸ�ϸ�, EnhancedInputLocalPlayerSubsystem�� �߰��Ѵ�
				for (const FDlkMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically)
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						// ���������� Input Mapping Context�� �߰��Ѵ�:
						// - AddPlayerMappableConfig�� ������ ���� ���� ��õ
						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
					}
				}

				UDlkInputComponent* DlkIC = CastChecked<UDlkInputComponent>(PlayerInputComponent);
				{
					// InputTag_Move�� InputTag_Look_Mouse�� ���� ���� Input_Move()�� Input_LookMouse() ��� �Լ��� ���ε���Ų��:
		// - ���ε��� ����, Input �̺�Ʈ�� ���� ��� �Լ��� Ʈ���ŵȴ�
					{
						TArray<uint32> BindHandles;
						DlkIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
					}
					// InputTag_Move�� InputTag_Look_Mouse�� ���� ���� Input_Move()�� Input_LookMouse() ��� �Լ��� ���ε���Ų��:
					// - ���ε��� ����, Input �̺�Ʈ�� ���� ��� �Լ��� Ʈ���ŵȴ�
					DlkIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					DlkIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				}
			}
		}
	}

	// GameFeatureAction_AddInputConfig�� HandlePawnExtension �ݹ� �Լ� ����
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
			// Left/Right -> X ���� �������:
			// MovementDirection�� ���� ī�޶��� RightVector�� �ǹ��� (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovementInput �Լ��� �ѹ� ����:
			// - ���������� MovementDirection * Value.X�� MovementComponent�� ����(���ϱ�)���ش�
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f) // �ռ� �츮�� Forward ������ ���� swizzle input modifier�� ����ߴ�~
		{
			// �ռ� Left/Right�� ���������� Forward/Backward�� �����Ѵ�
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
		// X���� Yaw ���� ����:
		// - Camera�� ���� Yaw ����
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y���� Pitch ��!
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
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
