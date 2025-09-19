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
#include "RuleRangerAction.h"
#include "EnsureDataOnlyBlueprintAction.generated.h"

/**
 * The structure for recording asset types that should have DataOnly blueprints.
 */
USTRUCT(BlueprintType)
struct FDataOnlyBlueprintEntry final : public FTableRowBase
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, meta = (AllowAbstract))
    TSoftClassPtr<UObject> ObjectType{ nullptr };
};

/**
 * Action to check that Blueprints assets that extend specific types are DataOnlyBlueprints.
 * The action also checks that any Blueprints that extend a UCLASS with the (boolean) meta property
 * 'RuleRangerDataOnly' must be a DataOnlyBlueprint.
 */
UCLASS(DisplayName = "Ensure Blueprints derived from specific types are Data Only Blueprints")
class RULERANGER_API UEnsureDataOnlyBlueprintAction final : public URuleRangerAction
{
    GENERATED_BODY()

    /** The array of tables that defines DataOnlyBlueprint types */
    UPROPERTY(EditAnywhere,
              meta = (RequiredAssetDataTags = "RowStructure=/Script/RuleRanger.DataOnlyBlueprintEntry",
                      ForceShowPluginContent = "true"))
    TArray<TObjectPtr<UDataTable>> DataOnlyBlueprintTables;

    /** Cache of DataTables from Config. */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDataTable>> ConfigConventionsTables;

    /** Cache for looking up rules. */
    TSet<TSoftClassPtr<UObject>> ConventionsCache;

    /** The types where subtypes MUST be DataOnlyBlueprints. */
    UPROPERTY(EditAnywhere, meta = (AllowAbstract = true))
    TArray<TSubclassOf<UObject>> ObjectTypes;

    /** Handle for delegate called when any object modified in editor. */
    FDelegateHandle OnObjectModifiedDelegateHandle;

    /** Callback when any object is modified in the editor. */
    void ResetCachesIfTablesModified(UObject* Object);

    /** Method to clear cache. */
    void ResetCaches();

    /** Method to build ConfigConventionsTables. */
    void RebuildConfigConventionsTables(const URuleRangerActionContext* ActionContext);

    /** Method to build convention cache if necessary. */
    void RebuildConventionCacheIfNecessary();

    void RebuildCachesIfNecessary();

public:
    virtual void Apply_Implementation(URuleRangerActionContext* ActionContext, UObject* Object) override;

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    virtual UClass* GetExpectedType() override;
};
