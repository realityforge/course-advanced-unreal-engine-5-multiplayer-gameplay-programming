using UnrealBuildTool;

public class ActionGame : ModuleRules
{
    public ActionGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Make sure we discover files from module directory
        PublicIncludePaths.Add("ActionGame/");

        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
        PrivateDependencyModuleNames.AddRange(new[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "Aeon" });
    }
}
