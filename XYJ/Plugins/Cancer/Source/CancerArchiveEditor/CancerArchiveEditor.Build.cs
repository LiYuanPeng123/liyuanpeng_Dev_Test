using UnrealBuildTool;
using System.Collections.Generic;

public class CancerArchiveEditor : ModuleRules
{
    public CancerArchiveEditor(ReadOnlyTargetRules Target) : base(Target)
    {
    
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "ToolMenus",
            "UnrealEd",
            "LevelEditor",
            "Kismet",
            "BlueprintGraph",
            "KismetCompiler",
            "CancerArchive"
        });
    }
}
