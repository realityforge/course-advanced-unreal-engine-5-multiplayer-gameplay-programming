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
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RuleRangerConfig.generated.h"

struct FRuleRangerRuleExclusion;
class URuleRangerRuleSet;

/**
 * A configuration which applies RuleSets to a set of Content Directories.
 */
UCLASS(AutoExpandCategories = ("Rule Ranger"), Blueprintable, BlueprintType, CollapseCategories, EditInlineNew)
class RULERANGER_API URuleRangerConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    /** The base content directory to which the RuleSet applies. */
    UPROPERTY(EditDefaultsOnly, Category = "Rule Ranger", meta = (ContentDir))
    TArray<FDirectoryPath> Dirs;

    /** A set of DataTables that are used to drive different actions. */
    UPROPERTY(EditDefaultsOnly,
              Category = "Rule Sets",
              meta = (DisplayThumbnail = "false", ForceShowPluginContent = "true"))
    TArray<TObjectPtr<UDataTable>> DataTables;

    /** A set of rule sets to be applied to directories. */
    UPROPERTY(EditDefaultsOnly,
              Category = "Rule Sets",
              meta = (AllowAbstract = "false", DisplayThumbnail = "false", ForceShowPluginContent = "true"))
    TArray<TObjectPtr<URuleRangerRuleSet>> RuleSets;

    /** A set of exclusions when applying rules. */
    UPROPERTY(EditDefaultsOnly,
              Category = "Rule Sets",
              meta = (AllowAbstract = "false",
                      DisplayThumbnail = "false",
                      ForceShowPluginContent = "true",
                      TitleProperty = "EditorFriendlyTitle"))
    TArray<FRuleRangerRuleExclusion> Exclusions;

    bool ConfigMatches(const FString& Path);

    /**
     * Collect the DataTables that are transitively defined in the RuleRangerConfig that have the specified
     * RowStructure.
     *
     * @param RowStructure The type representing each row in data table.
     * @param OutDataTables The variable in which to place matching DataTables.
     */
    void CollectDataTables(const UScriptStruct* RowStructure, TArray<TObjectPtr<UDataTable>>& OutDataTables) const;

    /**
     * Updates the editor only titles for subobjects.
     */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * Updates the editor only titles for subobjects.
     * This is called when properties that are inside of structs are modified.
     */
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

    /**
     * Updates the editor only titles for subobjects after the asset is initially loaded.
     */
    virtual void PostLoad() override;

private:
    void UpdateExclusionsEditorFriendlyTitles();
};
