using UnrealBuildTool;

public class ActionGameEditorTarget : TargetRules
{
    public ActionGameEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

        ExtraModuleNames.Add("ActionGame");
    }
}
