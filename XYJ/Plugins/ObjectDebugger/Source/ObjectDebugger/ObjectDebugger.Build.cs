using UnrealBuildTool;

public class ObjectDebugger : ModuleRules
{
    public ObjectDebugger(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd", 
                "EditorStyle",
                "InputCore",
                "LevelEditor",
                "ToolMenus",
                "ApplicationCore",
                "PropertyEditor"
            }
        );
    }
}
