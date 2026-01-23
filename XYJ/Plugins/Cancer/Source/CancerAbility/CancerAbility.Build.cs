// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CancerAbility : ModuleRules
{
	public CancerAbility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DeveloperSettings", 
				"GameplayTags", 
				"GameplayAbilities", 
				"GameplayTasks",
				"AIModule",
				"MotionWarping", 
				"Niagara", 
				"Niagara",
				"PhysicsCore",
				"ContextualAnimation",
				"TargetingSystem",
				"UMG",
				"EnhancedInput", 
				"AIModule",
				"CancerCore",
				"CancerCamera", 
				"Chooser", 
				"CancerInventory", 
				"TargetingSystem", 
				"CancerAnimation",
				"MovieScene",
				"EngineCameras",
				"GameplayCameras",
				"TemplateSequence",
				"LevelSequence", 
				"CancerUIExtension"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"CancerGame", 
				"CancerMessageRuntime", 
				"CancerCore",
				"CancerAsset", 
				"CancerAnimation",
				"CancerActorPool",
				"CancerInput", 
				"CancerTeam", 
				"CancerLockTarget",
				"CancerInventory"

				// ... add private dependencies that you statically link with here ...
			}
			);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "LevelEditor", "InputCore" });
		}
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
