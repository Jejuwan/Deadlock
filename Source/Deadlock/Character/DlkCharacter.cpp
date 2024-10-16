// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkCharacter.h"
#include "DlkPawnExtensionComponent.h"
#include "Deadlock/Camera/DlkCameraComponent.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "DlkHealthComponent.h"

// Sets default values
ADlkCharacter::ADlkCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick을 비활성화
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

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

UAbilitySystemComponent* ADlkCharacter::GetAbilitySystemComponent() const
{
	// 앞서, 우리는 PawnExtensionComponent에 AbilitySystemComponent를 캐싱하였다
	return PawnExtComponent->GetDlkAbilitySystemComponent();
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

