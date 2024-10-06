// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPlayerState.h"
#include "Deadlock/Gamemodes/DlkExperienceManagerComponent.h"
#include "Deadlock/Gamemodes/DlkGameModeBase.h"

void ADlkPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//check(AbilitySystemComponent);
	//// 아래의 코드는 우리가 InitAbilityActorInfo를 재호출을 통하는 이유를 설명하는 코드이다:
	//{
	//	// 처음 InitAbilityActorInfo를 호출 당시, OwnerActor와 AvatarActo가 같은 Actor를 가르키고 있으며, 이는 PlayerState이다
	//	// - OwnerActor는 PlayerState가 의도하는게 맞지만, AvatarActor는 PlayerController가 소유하는 대상인 Pawn이 되어야 한다!
	//	// - 이를 위해 재-세팅을 해준다
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
		// AHakGameMode에서 GetPawnDataForController를 구현해야 함
		// - GetPawnDataForController에서 우리는 아직 PawnData를 설정하지 않았으므로, ExperienceMangerComponent의 DefaultPawnData로 설정한다
		const UDlkPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void ADlkPlayerState::SetPawnData(const UDlkPawnData* InPawnData)
{
	check(InPawnData);

	// PawnData가 두번 설정되는 것은 원하지 않음!
	check(!PawnData);

	PawnData = InPawnData;
}
