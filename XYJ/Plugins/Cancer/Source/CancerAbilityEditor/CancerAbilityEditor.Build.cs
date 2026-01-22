using UnrealBuildTool;

public class CancerAbilityEditor : ModuleRules
{
    public CancerAbilityEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "CancerAbility",
            "GameplayTags"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "EditorStyle",
            "UnrealEd",
            "AssetTools",
            "ContentBrowser",
            "GraphEditor",
            "Kismet",
            "PropertyEditor",
            "ApplicationCore",
            "InputCore",
            "Projects"
        });
    }
}
