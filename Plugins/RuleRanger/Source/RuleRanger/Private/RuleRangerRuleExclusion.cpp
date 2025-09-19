/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "RuleRangerRuleExclusion.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RuleRangerRuleExclusion)

bool FRuleRangerRuleExclusion::ExclusionMatches(const UObject& Object, const FString& Path) const
{
    for (auto ObjectIt = Objects.CreateConstIterator(); ObjectIt; ++ObjectIt)
    {
        if (ObjectIt->Get() == &Object)
        {
            return true;
        }
    }
    for (auto DirIt = Dirs.CreateConstIterator(); DirIt; ++DirIt)
    {
        if (Path.StartsWith(DirIt->Path))
        {
            return true;
        }
    }
    return false;
}

FString FRuleRangerRuleExclusion::DeriveSuffix() const
{
    TArray<FString> Suffixes;
    if (RuleSets.Num() > 0)
    {
        Suffixes.Add(FString::Printf(TEXT("Excluded RuleSets=%d"), RuleSets.Num()));
    }
    if (Rules.Num() > 0)
    {
        Suffixes.Add(FString::Printf(TEXT("Excluded Rules=%d"), Rules.Num()));
    }
    if (Objects.Num() > 0)
    {
        Suffixes.Add(FString::Printf(TEXT("Matched Objects=%d"), Objects.Num()));
    }
    if (Objects.Num() > 0)
    {
        Suffixes.Add(FString::Printf(TEXT("Matched Dirs=%d"), Dirs.Num()));
    }
    if (Suffixes.IsEmpty())
    {
        Suffixes.Add(TEXT("Invalid Exclusion"));
    }
    return FString::Join(Suffixes, TEXT(", "));
}

void FRuleRangerRuleExclusion::InitEditorFriendlyTitleProperty()
{
    if (!Description.IsEmptyOrWhitespace())
    {
        EditorFriendlyTitle = Description.ToString().TrimStartAndEnd();
    }
    else if (1 == RuleSets.Num() + Rules.Num() && 1 == Objects.Num() + Dirs.Num())
    {
        const auto RuleMatcher = 1 == RuleSets.Num() ? RuleSets[0].GetName() : Rules[0].GetName();
        const auto TargetMatcher = 1 == Objects.Num() ? Objects[0].GetName() : Dirs[0].Path;
        EditorFriendlyTitle = FString::Printf(TEXT("Exclude %s from %s"), *RuleMatcher, *TargetMatcher);
    }
    else
    {
        const auto Suffix = DeriveSuffix();
        EditorFriendlyTitle = FString::Printf(TEXT("Undocumented Exclude (%s)"), *Suffix);
    }
}
