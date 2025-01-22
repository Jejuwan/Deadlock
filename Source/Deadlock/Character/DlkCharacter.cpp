// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DlkPawnExtensionComponent.h"
#include "Deadlock/Camera/DlkCameraComponent.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "DlkHealthComponent.h"

// Sets default values
ADlkCharacter::ADlkCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick을 비활성화
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UDlkAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// PawnExtComponent 생성
	PawnExtComponent = CreateDefaultSubobject<UDlkPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	{
		PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
		PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
	}

	// CameraComponent 생성
	{
		CameraComponent = CreateDefaultSubobject<UDlkCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}

	// HealthComponent 생성
	{
		HealthComponent = CreateDefaultSubobject<UDlkHealthComponent>(TEXT("HealthComponent"));
		HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
		HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
	}

}

void ADlkCharacter::OnAbilitySystemInitialized()
{
	UDlkAbilitySystemComponent* DlkASC = Cast<UDlkAbilitySystemComponent>(GetAbilitySystemComponent());
	check(DlkASC);

	// HealthComponent의 ASC를 통한 초기화
	HealthComponent->InitializeWithAbilitySystem(DlkASC);
}

void ADlkCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeWithAbilitySystem();
}

UDlkAbilitySystemComponent* ADlkCharacter::GetDlkAbilitySystemComponent() const
{
	return Cast<UDlkAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ADlkCharacter::GetAbilitySystemComponent() const
{
	// 앞서, 우리는 PawnExtensionComponent에 AbilitySystemComponent를 캐싱하였다
	return PawnExtComponent->GetDlkAbilitySystemComponent();
}

void ADlkCharacter::SetTeamColor(FColor color)
{
	TeamColor = color;
}

// Called when the game starts or when spawned
void ADlkCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADlkCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADlkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();

}

void ADlkCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent); 
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ADlkCharacter::OnDeathStarted(AActor* OwningActor)
{
	//DisableMovementAndCollision();
}

void ADlkCharacter::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ADlkCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* DlkMoveComp = GetCharacterMovement();
	DlkMoveComp->StopMovementImmediately();
	DlkMoveComp->DisableMovement();
}

void ADlkCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void ADlkCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UDlkAbilitySystemComponent* DlkASC = GetDlkAbilitySystemComponent())
	{
		if (DlkASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

