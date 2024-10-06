// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPlayerState.h"
#include "Deadlock/Gamemodes/DlkExperienceManagerComponent.h"
#include "Deadlock/Gamemodes/DlkGameModeBase.h"

void ADlkPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//check(AbilitySystemComponent);
	//// �Ʒ��� �ڵ�� �츮�� InitAbilityActorInfo�� ��ȣ���� ���ϴ� ������ �����ϴ� �ڵ��̴�:
	//{
	//	// ó�� InitAbilityActorInfo�� ȣ�� ���, OwnerActor�� AvatarActo�� ���� Actor�� ����Ű�� ������, �̴� PlayerState�̴�
	//	// - OwnerActor�� PlayerState�� �ǵ��ϴ°� ������, AvatarActor�� PlayerController�� �����ϴ� ����� Pawn�� �Ǿ�� �Ѵ�!
	//	// - �̸� ���� ��-������ ���ش�
	//	FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
	//	check(ActorInfo->OwnerActor == this);
	//	check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	//}
	//AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

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
		const UDlkPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void ADlkPlayerState::SetPawnData(const UDlkPawnData* InPawnData)
{
	check(InPawnData);

	// PawnData�� �ι� �����Ǵ� ���� ������ ����!
	check(!PawnData);

	PawnData = InPawnData;
}
