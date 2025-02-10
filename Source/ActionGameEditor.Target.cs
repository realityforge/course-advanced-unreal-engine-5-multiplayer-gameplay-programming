using UnrealBuildTool;
using System.Collections.Generic;

public class ActionGameEditorTarget : TargetRules
{
    public ActionGameEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("ActionGame");
    }
}
