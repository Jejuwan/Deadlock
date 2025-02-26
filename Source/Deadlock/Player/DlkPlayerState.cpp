// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPlayerState.h"
#include "Deadlock/Gamemodes/DlkExperienceManagerComponent.h"
#include "Deadlock/Gamemodes/DlkGameModeBase.h"
#include "Deadlock/System/DlkGameInstance.h"
#include "Deadlock/Character/DlkPawnData.h"
#include "Deadlock/AbilitySystem/DlkAbilitySet.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "Deadlock/AbilitySystem/Attributes/DlkCombatSet.h"
#include "Deadlock/AbilitySystem/Attributes/DlkHealthSet.h"
#include "Deadlock/AbilitySystem/Attributes/DlkBurntSet.h"
#include "Components/GameFrameworkComponentManager.h"

const FName ADlkPlayerState::NAME_DlkAbilityReady("DlkAbilitiesReady");

ADlkPlayerState::ADlkPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UDlkAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

 	CreateDefaultSubobject<UDlkHealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UDlkCombatSet>(TEXT("CombatSet"));
	CreateDefaultSubobject<UDlkBurntSet>(TEXT("BurntSet"));
}

void ADlkPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	// �Ʒ��� �ڵ�� �츮�� InitAbilityActorInfo�� ��ȣ���� ���ϴ� ������ �����ϴ� �ڵ��̴�:
	{
		// ó�� InitAbilityActorInfo�� ȣ�� ���, OwnerActor�� AvatarActo�� ���� Actor�� ����Ű�� ������, �̴� PlayerState�̴�
		// - OwnerActor�� PlayerState�� �ǵ��ϴ°� ������, AvatarActor�� PlayerController�� �����ϴ� ����� Pawn�� �Ǿ�� �Ѵ�!
		// - �̸� ���� ��-������ ���ش�
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	UDlkExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnDlkExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ADlkPlayerState::OnExperienceLoaded(const UDlkExperienceDefinition* CurrentExperience)
{
	if (ADlkGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ADlkGameModeBase>())
	{
		// AHakGameMode���� GetPawnDataForController�� �����ؾ� ��
		// - GetPawnDataForController���� �츮�� ���� PawnData�� �������� �ʾ����Ƿ�, ExperienceMangerComponent�� DefaultPawnData�� �����Ѵ�
		if(const UDlkPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
	}
}

void ADlkPlayerState::SetPawnData(const UDlkPawnData* InPawnData)
{
	check(InPawnData);

	// PawnData�� �ι� �����Ǵ� ���� ������ ����!
	check(!PawnData);

	PawnData = InPawnData;

	// PawnData�� AbilitySet�� ��ȸ�ϸ�, ASC�� Ability�� �Ҵ�(Give)�Ѵ�
// - �� �������� ASC�� ActivatableAbilities�� �߰��ȴ�
	for (UDlkAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_DlkAbilityReady);
}

UAbilitySystemComponent* ADlkPlayerState::GetAbilitySystemComponent() const
{
	return GetDlkAbilitySystemComponent();
}
