using UnrealBuildTool;
using System.Collections.Generic;

public class ActionGameTarget : TargetRules
{
    public ActionGameTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("ActionGame");
    }
}
