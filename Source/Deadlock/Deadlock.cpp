// Copyright Epic Games, Inc. All Rights Reserved.

#include "Deadlock.h"
#include "Modules/ModuleManager.h"
#include "Deadlock/System/DlkAssetManager.h"

class FDlkGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FDlkGameModule::StartupModule()
{
	UDlkAssetManager::TestClone();
}

void FDlkGameModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDlkGameModule, Deadlock, "Deadlock");

