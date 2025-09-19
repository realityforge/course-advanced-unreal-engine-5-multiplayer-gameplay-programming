using UnrealBuildTool;

public class ActionGameTarget : TargetRules
{
    public ActionGameTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

        ExtraModuleNames.Add("ActionGame");
    }
}
