// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameModeBase.h"
#include "DlkGameState.h"
#include "DlkExperienceDefinition.h"
#include "DlkExperienceManagerComponent.h"
#include "Deadlock/Character/DlkCharacter.h"
#include "Deadlock/Character/DlkPawnData.h"
#include "Deadlock/Character/DlkPawnExtensionComponent.h"
#include "Deadlock/Player/DlkPlayerController.h"
#include "Deadlock/Player/DlkPlayerState.h"
#include "Deadlock/Player/DlkPlayerBotController.h"
#include "Deadlock/Player/DlkTowerController.h"
#include "Deadlock/System/DlkGameInstance.h"
#include "Deadlock/UI/DlkHUD.h"
#include "Deadlock/DlkLogChannels.h"
#include "Kismet/GameplayStatics.h"


ADlkGameModeBase::ADlkGameModeBase()
{
	GameStateClass = ADlkGameState::StaticClass();
	PlayerControllerClass = ADlkPlayerController::StaticClass();
	PlayerStateClass = ADlkPlayerState::StaticClass();
	DefaultPawnClass = ADlkCharacter::StaticClass();
	HUDClass = ADlkHUD::StaticClass();
}

void ADlkGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// ���� GameInstance�� ����, �ʱ�ȭ �۾��� ����ǹǷ�, �� �����ӿ��� Lyra�� Concept�� Experience ó���� ������ �� ����:
	// - �̸� ó���ϱ� ����, �������� �ڿ� �̺�Ʈ�� �޾� ó���� �̾ �����Ѵ�
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ADlkGameModeBase::InitGameState()
{
	Super::InitGameState();

	// Experience �񵿱� �ε��� ���� Delegate�� �غ��Ѵ�:
	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded ���
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnDlkExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* ADlkGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController�� Ȱ���Ͽ�, PawnData�κ��� PawnClass�� ��������
	if (const UDlkPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ADlkGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if(IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* ADlkGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			// FindPawnExtensionComponent ����
			if (UDlkPawnExtensionComponent* PawnExtComp = UDlkPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UDlkPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}

	return nullptr;
}


void ADlkGameModeBase::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	/*else if (ADlkPlayerBotController* BotController = Cast<ADlkPlayerBotController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(BotController, &ADlkPlayerBotController::ServerRestartController);
	}*/
}

bool ADlkGameModeBase::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	return true;
}

void ADlkGameModeBase::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);
}

void ADlkGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// �ش� �Լ������� �츮�� �ε��� Experience�� ���� PrimaryAssetId�� �����Ͽ�, OnMatchAssignmentGiven���� �Ѱ��ش�

	FPrimaryAssetId ExperienceId;

	// precedence order (highest wins)
	// - matchmaking assignment (if present)
	// - default experience

	UWorld* World = GetWorld();

	// �츮�� �ռ�, URL�� �Բ� ExtraArgs�� �Ѱ�� ������ OptionsString�� �����Ǿ� �ִ�.
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// Experience�� Value�� �����ͼ�, PrimaryAssetId�� �������ش�: �̶�, HakExperienceDefinition�� Class �̸��� ����Ѵ�
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UDlkExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	// fall back to the default experience
	// �ϴ� �⺻ �ɼ����� default�ϰ� B_HakDefaultExperience�� ��������
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("DlkExperienceDefinition"), FName("B_DlkDefaultExperience"));
	}

	// ���ڰ� ������ HandleMatchAssignmentIfNotExpectingOne�� OnMatchAssignmentGiven()�� ���� ���������� �̸��� �ʹ��� �ʴ´ٰ� �����Ѵ�
	// - ����, ������� Lyra�� �����Ǹ�, �ش� �Լ��� ���� �� ������ �� ���� ������ �����Ѵ�
	OnMatchAssignmentGiven(ExperienceId);
}

void ADlkGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// �ش� �Լ��� ExperienceManagerComponent�� Ȱ���Ͽ� Experience�� �ε��ϱ� ����, ExperienceManagerComponent�� ServerSetCurrentExperience�� ȣ���Ѵ�

	check(ExperienceId.IsValid());

	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool ADlkGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void ADlkGameModeBase::OnExperienceLoaded(const UDlkExperienceDefinition* CurrentExperience)
{
	//PlayerController�� ��ȸ�ϸ�
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController�� Pawn�� Possess���� �ʾҴٸ�, RestartPlayer�� ���� Pawn�� �ٽ� Spawn�Ѵ�
		// - �ѹ� OnPossess�� ������ ����:
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

const UDlkPawnData* ADlkGameModeBase::GetPawnDataForController(const AController* InController) const
{
	// ���� ���߿� PawnData�� �������̵� �Ǿ��� ���, PawnData�� PlayerState���� �������� ��
	if (InController)
	{
		if (const ADlkPlayerState* HakPS = InController->GetPlayerState<ADlkPlayerState>())
		{
			// GetPawnData ����
			if (const UDlkPawnData* PawnData = HakPS->GetPawnData<UDlkPawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current experience
	// ���� PlayerState�� PawnData�� �����Ǿ� ���� ���� ���, ExperienceManagerComponent�� CurrentExperience�κ��� �����ͼ� ����
	check(GameState);
	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked ����
		const UDlkExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();

		if(const ADlkPlayerController* PlayerController = Cast<ADlkPlayerController>(InController))
		{
			if (Experience->FirstCharacterPawnData)
			{
					if (const UDlkGameInstance* Instance = GetWorld()->GetGameInstance<UDlkGameInstance>())
					{
						switch (Instance->CharacterType)
						{
						case EDlkCharacterType::Manny:
							return Experience->FirstCharacterPawnData;
							break;
						case EDlkCharacterType::Quinn:
							return Experience->SecondCharacterPawnData;
							break;
						}
					}
				return Experience->FirstCharacterPawnData;
			}
		}
		else if (const ADlkPlayerBotController* BotController = Cast<ADlkPlayerBotController>(InController))
		{
			if (Experience->MeleeMinionData)
			{
				return Experience->MeleeMinionData;
			}
		}
		else if (const ADlkTowerController* TowerController = Cast<ADlkTowerController>(InController))
		{
			if (Experience->TowerData)
			{
				return Experience->TowerData;
			}
		}
	}

	// ��� ���̽����� �ڵ鸵 �ȵǾ����� nullptr
	return nullptr;
}
