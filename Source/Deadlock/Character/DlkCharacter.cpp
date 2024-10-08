// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkCharacter.h"
#include "DlkPawnExtensionComponent.h"
#include "Deadlock/Camera/DlkCameraComponent.h"

// Sets default values
ADlkCharacter::ADlkCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick阑 厚劝己拳
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// PawnExtComponent 积己
	PawnExtComponent = CreateDefaultSubobject<UDlkPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// CameraComponent 积己
	{
		CameraComponent = CreateDefaultSubobject<UDlkCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}

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

