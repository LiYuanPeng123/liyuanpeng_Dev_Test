
using UnrealBuildTool;

public class XYJ : ModuleRules
{
	public XYJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"AIModule",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"UMG",
			"SlateCore",
			"Chooser"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"CancerAnimation", 
			"CancerGameplay", 
			"CancerInput", 
			"CancerCore", 
			"CancerInventory", 
			"CancerAbility",
			"CancerUI", 
			"CancerAI",
			"CancerTeam", 
			"CancerIndicator",
			"CancerActionCreator",
			"CancerAsset",
			"CancerActorPool",
			"CancerCamera",
			"CancerLockTarget",
			"CancerNumberPop",
			"CancerUIExtension",
			"CancerGame",
			"CancerTrigger"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}