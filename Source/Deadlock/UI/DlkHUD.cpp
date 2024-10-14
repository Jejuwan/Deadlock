#include "DlkHUD.h"
#include "Components/GameFrameworkComponentManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkHUD)

ADlkHUD::ADlkHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void ADlkHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// DlkHUD�� Receiver�� Actor�� �߰�����
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void ADlkHUD::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void ADlkHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}
