using UnrealBuildTool;

public class CancerMontageTools : ModuleRules
{
    public CancerMontageTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "UnrealEd",
            "LevelEditor",
            "InputCore",
            "EditorFramework"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "InputCore",
            "UnrealEd",
            "LevelEditor",
            "EditorFramework"
        });
    }
}
