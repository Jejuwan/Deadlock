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

	// 아직 GameInstance를 통해, 초기화 작업이 진행되므로, 현 프레임에는 Lyra의 Concept인 Experience 처리를 진행할 수 없다:
	// - 이를 처리하기 위해, 한프레임 뒤에 이벤트를 받아 처리를 이어서 진행한다
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ADlkGameModeBase::InitGameState()
{
	Super::InitGameState();

	// Experience 비동기 로딩을 위한 Delegate를 준비한다:
	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded 등록
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnDlkExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* ADlkGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController를 활용하여, PawnData로부터 PawnClass를 유도하자
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
			// FindPawnExtensionComponent 구현
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
	// 해당 함수에서는 우리가 로딩할 Experience에 대해 PrimaryAssetId를 생성하여, OnMatchAssignmentGiven으로 넘겨준다

	FPrimaryAssetId ExperienceId;

	// precedence order (highest wins)
	// - matchmaking assignment (if present)
	// - default experience

	UWorld* World = GetWorld();

	// 우리가 앞서, URL과 함께 ExtraArgs로 넘겼던 정보는 OptionsString에 저정되어 있다.
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// Experience의 Value를 가져와서, PrimaryAssetId를 생성해준다: 이때, HakExperienceDefinition의 Class 이름을 사용한다
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UDlkExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	// fall back to the default experience
	// 일단 기본 옵션으로 default하게 B_HakDefaultExperience로 설정놓자
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("DlkExperienceDefinition"), FName("B_DlkDefaultExperience"));
	}

	// 필자가 이해한 HandleMatchAssignmentIfNotExpectingOne과 OnMatchAssignmentGiven()은 아직 직관적으로 이름이 와닫지 않는다고 생각한다
	// - 후일, 어느정도 Lyra가 구현되면, 해당 함수의 명을 더 이해할 수 있을 것으로 예상한다
	OnMatchAssignmentGiven(ExperienceId);
}

void ADlkGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// 해당 함수는 ExperienceManagerComponent을 활용하여 Experience을 로딩하기 위해, ExperienceManagerComponent의 ServerSetCurrentExperience를 호출한다

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
	//PlayerController를 순회하며
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController가 Pawn을 Possess하지 않았다면, RestartPlayer를 통해 Pawn을 다시 Spawn한다
		// - 한번 OnPossess를 보도록 하자:
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
	// 게임 도중에 PawnData가 오버라이드 되었을 경우, PawnData는 PlayerState에서 가져오게 됨
	if (InController)
	{
		if (const ADlkPlayerState* HakPS = InController->GetPlayerState<ADlkPlayerState>())
		{
			// GetPawnData 구현
			if (const UDlkPawnData* PawnData = HakPS->GetPawnData<UDlkPawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current experience
	// 아직 PlayerState에 PawnData가 설정되어 있지 않은 경우, ExperienceManagerComponent의 CurrentExperience로부터 가져와서 설정
	check(GameState);
	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked 구현
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

	// 어떠한 케이스에도 핸들링 안되었으면 nullptr
	return nullptr;
}
