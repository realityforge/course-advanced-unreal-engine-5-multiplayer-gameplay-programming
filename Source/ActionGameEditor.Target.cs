using UnrealBuildTool;
using System.Collections.Generic;

public class ActionGameEditorTarget : TargetRules
{
    public ActionGameEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("ActionGame");
    }
}
