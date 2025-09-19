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
#include "EnsureDataOnlyBlueprintAction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleRanger/RuleRangerUtilities.h"
#include "RuleRangerConfig.h"
#include "RuleRangerRuleSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnsureDataOnlyBlueprintAction)

static const FName RuleRangerDataOnlyPropertyName("RuleRangerDataOnly");

static FString GetObjectTypesAsString(const TArray<TSubclassOf<UObject>>& ObjectTypes)
{
    TArray<FString> ClassNames;
    ClassNames.Reserve(ObjectTypes.Num());

    for (const auto& ObjectType : ObjectTypes)
    {
        if (ObjectType)
        {
            ClassNames.Add(ObjectType->GetPathName());
        }
    }

    return FString::Join(ClassNames, TEXT(", "));
}

void UEnsureDataOnlyBlueprintAction::ResetCachesIfTablesModified(UObject* Object)
{
    // This is called on any object edit in editor so match against tables and bust cache as appropriate
    if (Object && DataOnlyBlueprintTables.Contains(Object))
    {
        ResetCaches();
    }
}

void UEnsureDataOnlyBlueprintAction::ResetCaches()
{
    LogInfo(nullptr, TEXT("Resetting the Name Convention Caches"));

    ConventionsCache.Empty();
    FCoreUObjectDelegates::OnObjectModified.Remove(OnObjectModifiedDelegateHandle);
    OnObjectModifiedDelegateHandle.Reset();
}

void UEnsureDataOnlyBlueprintAction::RebuildConfigConventionsTables(const URuleRangerActionContext* ActionContext)
{
    ConfigConventionsTables.Reset();
    ActionContext->GetOwnerConfig()->CollectDataTables(FDataOnlyBlueprintEntry::StaticStruct(),
                                                       ConfigConventionsTables);
    for (const auto DataTable : ConfigConventionsTables)
    {
        LogInfo(nullptr,
                FString::Printf(TEXT("Adding DataTable '%s' registered in Config %s to set "
                                     "of DataOnlyBlueprint rules applied"),
                                *DataTable.GetName(),
                                *ActionContext->GetOwnerConfig()->GetName()));
    }
}

void UEnsureDataOnlyBlueprintAction::RebuildConventionCacheIfNecessary()
{
    if (DataOnlyBlueprintTables.IsEmpty() && ConfigConventionsTables.IsEmpty() && !ConventionsCache.IsEmpty())
    {
        ConventionsCache.Reset();
    }

    TArray<TObjectPtr<UDataTable>> ConventionsTables;
    ConventionsTables.Append(DataOnlyBlueprintTables);
    ConventionsTables.Append(ConfigConventionsTables);
    bool bTableDataPresent = false;
    for (const auto& NameConventionsTable : ConventionsTables)
    {
        if (NameConventionsTable && 0 != NameConventionsTable->GetTableData().Num())
        {
            bTableDataPresent = true;
            break;
        }
    }
    if (ConventionsCache.IsEmpty() && bTableDataPresent)
    {
        for (const auto& ConventionsTable : ConventionsTables)
        {
            if (IsValid(ConventionsTable))
            {
                for (const auto& RowName : ConventionsTable->GetRowNames())
                {
                    const auto& Convention = ConventionsTable->FindRow<FDataOnlyBlueprintEntry>(RowName, TEXT(""));
                    // ReSharper disable once CppTooWideScopeInitStatement
                    const auto& ObjectType = Convention->ObjectType.Get();
                    if (Convention && IsValid(ObjectType))
                    {
                        ConventionsCache.Add(ObjectType);
                    }
                }
            }
        }
        LogInfo(nullptr,
                FString::Printf(TEXT("ConventionsCache rebuilt: %d entries in cache"), ConventionsCache.Num()));
    }
}

void UEnsureDataOnlyBlueprintAction::RebuildCachesIfNecessary()
{
    RebuildConventionCacheIfNecessary();

    if (!OnObjectModifiedDelegateHandle.IsValid())
    {
        // Add a callback for when ANY object is modified in the editor so that we can bust the cache
        OnObjectModifiedDelegateHandle =
            FCoreUObjectDelegates::OnObjectModified.AddUObject(this, &ThisClass::ResetCachesIfTablesModified);
    }
}

void UEnsureDataOnlyBlueprintAction::Apply_Implementation(URuleRangerActionContext* ActionContext, UObject* Object)
{
    RebuildConfigConventionsTables(ActionContext);
    RebuildCachesIfNecessary();

    bool bMatchedViaMetaProperty = false;
    bool bMatchedViaDataTableEntry = false;
    TSubclassOf<UObject> MatchedObjectType{ nullptr };
    for (const auto ObjectType : ObjectTypes)
    {
        if (FRuleRangerUtilities::IsA(Object, ObjectType))
        {
            MatchedObjectType = ObjectType;
        }
    }
    if (!MatchedObjectType)
    {
        for (const auto SoftObjectType : ConventionsCache)
        {
            if (SoftObjectType.IsValid())
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                const auto ObjectType = SoftObjectType.Get();
                if (ObjectType && FRuleRangerUtilities::IsA(Object, ObjectType))
                {
                    MatchedObjectType = ObjectType;
                    bMatchedViaDataTableEntry = true;
                    break;
                }
            }
        }
    }
    if (!MatchedObjectType)
    {
        TArray<UClass*> Classes;
        FRuleRangerUtilities::CollectTypeHierarchy(Object, Classes);
        for (const auto Class : Classes)
        {
            if (Class->HasMetaData(RuleRangerDataOnlyPropertyName))
            {
                MatchedObjectType = Class;
                bMatchedViaMetaProperty = true;
                break;
            }
        }
    }

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto Blueprint = CastChecked<UBlueprint>(Object);
    if (MatchedObjectType && !FBlueprintEditorUtils::IsDataOnlyBlueprint(Blueprint))
    {
        if (bMatchedViaMetaProperty)
        {
            ActionContext->Error(FText::FromString(
                FString::Printf(TEXT("Object is not a DataOnlyBlueprint but it extends the type %s that has "
                                     "the meta property '%s'"),
                                *MatchedObjectType->GetPathName(),
                                *RuleRangerDataOnlyPropertyName.ToString())));
        }
        else if (bMatchedViaDataTableEntry)
        {
            ActionContext->Error(FText::FromString(
                FString::Printf(TEXT("Object is not a DataOnlyBlueprint but it extends the type %s that has "
                                     "been registered in a DataTable as expecting DataOnlyBlueprint subtypes"),
                                *MatchedObjectType->GetPathName())));
        }
        else
        {
            ActionContext->Error(FText::FromString(
                FString::Printf(TEXT("Object is not a DataOnlyBlueprint but it extends the type %s that is "
                                     "part of the list of DataOnlyBlueprints: %s"),
                                *MatchedObjectType->GetPathName(),
                                *GetObjectTypesAsString(ObjectTypes))));
        }
    }
}

void UEnsureDataOnlyBlueprintAction::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    // ReSharper disable once CppTooWideScopeInitStatement
    const auto PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    if ((GET_MEMBER_NAME_CHECKED(ThisClass, DataOnlyBlueprintTables)) == PropertyName)
    {
        ResetCaches();
    }
    else if ((GET_MEMBER_NAME_CHECKED(URuleRangerConfig, DataTables)) == PropertyName)
    {
        ResetCaches();
    }
    else if ((GET_MEMBER_NAME_CHECKED(URuleRangerRuleSet, DataTables)) == PropertyName)
    {
        ResetCaches();
    }
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

UClass* UEnsureDataOnlyBlueprintAction::GetExpectedType()
{
    return UBlueprint::StaticClass();
}
