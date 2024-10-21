// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Deadlock : ModuleRules
{
	public Deadlock(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
		"Core",
		"CoreUObject",
		"Engine",
		"InputCore",
        "PhysicsCore",
        "AIModule",
        "EnhancedInput",
		//Gas
		"GameplayTags",
		"GameplayTasks",
		"GameplayAbilities",
		//Game Features
		"ModularGameplay",
        "GameFeatures",
        "ModularGameplayActors",
		//Input
		"InputCore",
		"EnhancedInput",
		//CommonUser
		"CommonUser",
		"CommonGame",
		"CommonUI",
		"UMG",
		"UIExtension",
		"Slate",
		"SlateCore",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
        "GameplayMessageRuntime",
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
