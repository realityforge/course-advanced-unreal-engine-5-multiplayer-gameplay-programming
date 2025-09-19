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
#include "Aeon/AbilitySystem/AeonAbilitySet.h"
#include "ActiveGameplayEffectHandle.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"
#include "Aeon/AbilitySystem/AeonGameplayAbility.h"
#include "Aeon/Logging.h"
#include "Logging/StructuredLog.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonAbilitySet)

#if WITH_EDITOR
static FString GameplayTagContainerToString(const FGameplayTagContainer& TagContainer)
{
    TArray<FString> TagNames;
    for (const auto& Tag : TagContainer)
    {
        TagNames.Add(Tag.ToString());
    }

    return FString::Join(TagNames, TEXT(","));
}

void FAeonGameplayAbilityEntry::InitEditorFriendlyTitleProperty()
{
    if (Ability)
    {
        const auto Package = Ability->GetOuterUPackage();
        check(Package);
        if (Ability->IsInBlueprint())
        {
            EditorFriendlyTitle = FString::Printf(TEXT("%s [%d] %s"),
                                                  *FPackageName::GetShortName(Package),
                                                  Level,
                                                  *GameplayTagContainerToString(InputTags));
        }
        else
        {
            EditorFriendlyTitle = FString::Printf(TEXT("%s.%s [%d] %s"),
                                                  *FPackageName::GetShortName(Package),
                                                  *Ability->GetName(),
                                                  Level,
                                                  *GameplayTagContainerToString(InputTags));
        }
    }
    else
    {
        EditorFriendlyTitle = TEXT("None");
    }
}

void FAeonGameplayEffectEntry::InitEditorFriendlyTitleProperty()
{
    if (Effect)
    {
        const auto Package = Effect->GetOuterUPackage();
        check(Package);
        if (Effect->IsInBlueprint())
        {
            EditorFriendlyTitle = FString::Printf(TEXT("%s [%d]"), *FPackageName::GetShortName(Package), Level);
        }
        else
        {
            EditorFriendlyTitle =
                FString::Printf(TEXT("%s.%s [%d]"), *FPackageName::GetShortName(Package), *Effect->GetName(), Level);
        }
    }
    else
    {
        EditorFriendlyTitle = TEXT("None");
    }
}

void FAeonAttributeSetEntry::InitEditorFriendlyTitleProperty()
{
    if (AttributeSet)
    {
        const auto Package = AttributeSet->GetOuterUPackage();
        check(Package);
        if (AttributeSet->IsInBlueprint())
        {
            EditorFriendlyTitle = FPackageName::GetShortName(Package);
        }
        else
        {
            EditorFriendlyTitle =
                FString::Printf(TEXT("%s.%s"), *FPackageName::GetShortName(Package), *AttributeSet->GetName());
        }
    }
    else
    {
        EditorFriendlyTitle = TEXT("None");
    }
}

void FAeonAttributeInitializer::InitEditorFriendlyTitleProperty()
{
    EditorFriendlyTitle =
        FString::Printf(TEXT("%s [%d] = %.2f"), *Attribute.GetName(), Level, Value.GetValueAtLevel(Level));
}
#endif

void FAeonAbilitySetHandles::RemoveFromAbilitySystemComponent()
{
    if (AbilitySystemComponent)
    {
        if (AbilitySystemComponent->IsOwnerActorAuthoritative())
        {
            for (const auto& Handle : AbilitySpecHandles)
            {
                if (Handle.IsValid())
                {
                    AbilitySystemComponent->ClearAbility(Handle);
                }
            }

            for (const auto& Handle : EffectHandles)
            {
                if (Handle.IsValid())
                {
                    AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
                }
            }

            for (const auto AttributeSet : AttributeSets)
            {
                AbilitySystemComponent->RemoveSpawnedAttribute(AttributeSet);
            }

            AbilitySystemComponent->RemoveLooseGameplayTags(Tags);

            AbilitySpecHandles.Reset();
            EffectHandles.Reset();
            AttributeSets.Reset();
            Tags.Reset();
            AbilitySystemComponent = nullptr;
        }
        else
        {
            UE_LOGFMT(LogAeon,
                      Warning,
                      "RemoveAbilitySetFromAbilitySystemComponent() must be invoked when "
                      "OwnerActor is Authoritative");
        }
    }
    else
    {
        UE_LOGFMT(LogAeon,
                  Warning,
                  "RemoveAbilitySetFromAbilitySystemComponent() invoked when AbilitySystemComponent "
                  "is invalid. This is likely a result of invoking it multiple times. Please guard "
                  "call with IsValid() or avoid calling when handles are invalid.");
    }
}

bool FAeonAbilitySetHandles::IsValid() const
{
    return nullptr != AbilitySystemComponent;
}

UAeonAbilitySet::UAeonAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void UAeonAbilitySet::GrantToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent,
                                           FAeonAbilitySetHandles* OutGrantedHandles,
                                           int32 LevelDelta,
                                           UObject* SourceObject) const
{
    checkf(AbilitySystemComponent, TEXT("AbilitySystemComponent must not be null"));
    if (AbilitySystemComponent->IsOwnerActorAuthoritative())
    {
        if (OutGrantedHandles)
        {
            OutGrantedHandles->AbilitySystemComponent = AbilitySystemComponent;
        }

        if (!Tags.IsEmpty())
        {
            if (UE_LOG_ACTIVE(LogAeon, Error))
            {
                for (int32 Index = 0; Index < Tags.Num(); ++Index)
                {
                    if (const auto Tag = Tags.GetByIndex(Index); !Tag.IsValid())
                    {
                        UE_LOGFMT(LogAeon,
                                  Error,
                                  "AbilitySet '{AbilitySet}' has invalid tag at Tags[{Tags}]",
                                  GetNameSafe(this),
                                  Index);
                    }
                }
            }
            AbilitySystemComponent->AddLooseGameplayTags(Tags);
            if (OutGrantedHandles)
            {
                OutGrantedHandles->Tags = Tags;
            }
        }

        for (int32 Index = 0; Index < AttributeSets.Num(); ++Index)
        {
            // ReSharper disable once CppUseStructuredBinding
            if (const auto& Entry = AttributeSets[Index]; IsValid(Entry.AttributeSet))
            {
                const auto Outer = AbilitySystemComponent->GetOwner();
                const auto AttributeSet = NewObject<UAttributeSet>(Outer, Entry.AttributeSet);
                AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet);
                if (OutGrantedHandles)
                {
                    OutGrantedHandles->AttributeSets.Add(AttributeSet);
                }
            }
            else
            {
                UE_LOGFMT(LogAeon,
                          Error,
                          "AbilitySet '{AbilitySet}' has invalid value at AttributeSets[{Index}]",
                          GetNameSafe(this),
                          Index);
            }
        }

        for (int32 Index = 0; Index < Abilities.Num(); ++Index)
        {
            // ReSharper disable once CppUseStructuredBinding
            if (const auto& Entry = Abilities[Index]; IsValid(Entry.Ability))
            {
                const auto CDO = Entry.Ability->GetDefaultObject<UGameplayAbility>();
                FGameplayAbilitySpec AbilitySpec(CDO, Entry.Level + LevelDelta);
                AbilitySpec.SourceObject = SourceObject;
                for (auto InputTag : Entry.InputTags)
                {
                    // Only add tag if it is valid
                    AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
                }

                // ReSharper disable once CppTooWideScopeInitStatement
                const auto Handle = AbilitySystemComponent->GiveAbility(AbilitySpec);
                if (OutGrantedHandles && Handle.IsValid())
                {
                    OutGrantedHandles->AbilitySpecHandles.Add(Handle);
                }
            }
            else
            {
                UE_LOGFMT(LogAeon,
                          Error,
                          "AbilitySet '{AbilitySet}' has invalid value at Abilities[{Index}]",
                          GetNameSafe(this),
                          Index);
            }
        }

        for (int32 Index = 0; Index < Effects.Num(); ++Index)
        {
            // ReSharper disable once CppUseStructuredBinding
            if (const auto& Entry = Effects[Index]; IsValid(Entry.Effect))
            {
                const auto CDO = Entry.Effect->GetDefaultObject<UGameplayEffect>();
                auto EffectContext = AbilitySystemComponent->MakeEffectContext();
                EffectContext.AddSourceObject(SourceObject);
                const float EffectLevel = Entry.Level + LevelDelta;
                // ReSharper disable once CppTooWideScopeInitStatement
                const auto Handle = AbilitySystemComponent->ApplyGameplayEffectToSelf(CDO, EffectLevel, EffectContext);
                if (OutGrantedHandles && Handle.IsValid())
                {
                    OutGrantedHandles->EffectHandles.Add(Handle);
                }
            }
            else
            {
                UE_LOGFMT(LogAeon,
                          Error,
                          "AbilitySet '{AbilitySet}' has invalid value at Effects[{Index}]",
                          GetNameSafe(this),
                          Index);
            }
        }

        for (int32 Index = 0; Index < AttributeValues.Num(); ++Index)
        {
            // ReSharper disable once CppUseStructuredBinding
            if (const auto& Entry = AttributeValues[Index]; Entry.Attribute.IsValid() && Entry.Value.IsValid())
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                const FGameplayAttribute& Attribute = Entry.Attribute;
                if (AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
                {
                    const float Level = Entry.Level + LevelDelta;
                    AbilitySystemComponent->SetNumericAttributeBase(Attribute, Entry.Value.GetValueAtLevel(Level));
                }
                else
                {
                    UE_LOGFMT(LogAeon,
                              Error,
                              "AbilitySet '{AbilitySet}' has an attribute initializer '{Attribute}' for an "
                              "attribute from an AttributeSet '{AttributeSet}' that is not granted to the "
                              "AbilitySystemComponent at AttributeValues[{Index}]",
                              GetNameSafe(this),
                              Attribute.GetName(),
                              Attribute.GetAttributeSetClass()->GetName(),
                              Index);
                }
            }
            else
            {
                UE_LOGFMT(LogAeon,
                          Error,
                          "AbilitySet '{AbilitySet}' has invalid value at AttributeValues[{Index}]",
                          GetNameSafe(this),
                          Index);
            }
        }
    }
    else
    {
        UE_LOGFMT(LogAeon, Warning, "GiveToAbilitySystem() must be invoked when OwnerActor is Authoritative");
    }
}

#if WITH_EDITOR
EDataValidationResult UAeonAbilitySet::IsDataValid(FDataValidationContext& Context) const
{
    auto Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
    for (int32 Index = 0; Index < Tags.Num(); ++Index)
    {
        if (const auto Tag = Tags.GetByIndex(Index); !Tag.IsValid())
        {
            Context.AddError(FText::FromString(FString::Printf(TEXT("Tags[%d] is an invalid tag"), Index)));
            Result = EDataValidationResult::Invalid;
        }
    }

    for (int32 Index = 0; Index < Abilities.Num(); ++Index)
    {
        // ReSharper disable once CppUseStructuredBinding
        if (const auto& Ability = Abilities[Index]; !IsValid(Ability.Ability))
        {
            Context.AddError(
                FText::FromString(FString::Printf(TEXT("Abilities[%d].Ability references an invalid value"), Index)));
            Result = EDataValidationResult::Invalid;
        }
    }

    for (int32 Index = 0; Index < Effects.Num(); ++Index)
    {
        // ReSharper disable once CppUseStructuredBinding
        if (const auto& Effect = Effects[Index]; !IsValid(Effect.Effect))
        {
            Context.AddError(
                FText::FromString(FString::Printf(TEXT("Effects[%d].Effect references an invalid value"), Index)));
            Result = EDataValidationResult::Invalid;
        }
    }

    for (int32 Index = 0; Index < AttributeSets.Num(); ++Index)
    {
        // ReSharper disable once CppUseStructuredBinding
        if (const auto& Entry = AttributeSets[Index]; !IsValid(Entry.AttributeSet))
        {
            Context.AddError(FText::FromString(
                FString::Printf(TEXT("AttributeSets[%d].AttributeSet references an invalid value"), Index)));
            Result = EDataValidationResult::Invalid;
        }
    }

    for (int32 Index = 0; Index < AttributeValues.Num(); ++Index)
    {
        // ReSharper disable once CppUseStructuredBinding
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& Entry = AttributeValues[Index];
        if (!Entry.Attribute.IsValid())
        {
            Context.AddError(FText::FromString(
                FString::Printf(TEXT("AttributeValues[%d].Attribute references an invalid value"), Index)));
            Result = EDataValidationResult::Invalid;
        }
        if (!Entry.Value.IsValid())
        {
            Context.AddError(FText::FromString(
                FString::Printf(TEXT("AttributeValues[%d].Value references an invalid value"), Index)));
            Result = EDataValidationResult::Invalid;
        }
        bool bAttributeFound = false;

        for (int32 AttributeSetIndex = 0; AttributeSetIndex < AttributeSets.Num(); ++AttributeSetIndex)
        {
            // ReSharper disable once CppUseStructuredBinding
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto& AttributeSetEntry = AttributeSets[AttributeSetIndex];
            if (IsValid(AttributeSetEntry.AttributeSet)
                && AttributeSetEntry.AttributeSet.Get() == Entry.Attribute.GetAttributeSetClass())
            {
                bAttributeFound = true;
            }
        }
        if (!bAttributeFound)
        {
            Context.AddError(
                FText::FromString(FString::Printf(TEXT("AttributeValues[%d].Attribute named '%s' references an "
                                                       "AttributeSet '%s' that is not "
                                                       "defined in the AttributeSets property"),
                                                  Index,
                                                  *Entry.Attribute.GetName(),
                                                  *Entry.Attribute.GetAttributeSetClass()->GetName())));
            Result = EDataValidationResult::Invalid;
        }
    }

    return Result;
}

void UAeonAbilitySet::UpdateAbilityEditorFriendlyTitles()
{
    for (int32 Index = 0; Index < Abilities.Num(); ++Index)
    {
        if (auto& Ability = Abilities[Index]; IsValid(Ability.Ability))
        {
            Ability.InitEditorFriendlyTitleProperty();
        }
    }
}

void UAeonAbilitySet::UpdateEffectEditorFriendlyTitles()
{
    for (int32 Index = 0; Index < Effects.Num(); ++Index)
    {
        if (auto& Effect = Effects[Index]; IsValid(Effect.Effect))
        {
            Effect.InitEditorFriendlyTitleProperty();
        }
    }
}

void UAeonAbilitySet::UpdateAttributeSetEditorFriendlyTitles()
{
    for (int32 Index = 0; Index < AttributeSets.Num(); ++Index)
    {
        if (auto& AttributeSet = AttributeSets[Index]; IsValid(AttributeSet.AttributeSet))
        {
            AttributeSet.InitEditorFriendlyTitleProperty();
        }
    }
}

void UAeonAbilitySet::UpdateAttributeValueEditorFriendlyTitles()
{
    for (int32 Index = 0; Index < AttributeValues.Num(); ++Index)
    {
        if (auto& AttributeValue = AttributeValues[Index]; AttributeValue.Attribute.IsValid())
        {
            AttributeValue.InitEditorFriendlyTitleProperty();
        }
    }
}

void UAeonAbilitySet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto PropertyName = PropertyChangedEvent.Property->GetFName();

        if ((GET_MEMBER_NAME_CHECKED(ThisClass, Abilities)) == PropertyName)
        {
            UpdateAbilityEditorFriendlyTitles();
        }
        else if ((GET_MEMBER_NAME_CHECKED(ThisClass, Effects)) == PropertyName)
        {
            UpdateEffectEditorFriendlyTitles();
        }
        else if ((GET_MEMBER_NAME_CHECKED(ThisClass, AttributeSets)) == PropertyName)
        {
            UpdateAttributeSetEditorFriendlyTitles();
        }
        else if ((GET_MEMBER_NAME_CHECKED(ThisClass, AttributeValues)) == PropertyName)
        {
            UpdateAttributeValueEditorFriendlyTitles();
        }
    }
}

void UAeonAbilitySet::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto PropertyName = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName();

    if ((GET_MEMBER_NAME_CHECKED(ThisClass, Abilities)) == PropertyName)
    {
        UpdateAbilityEditorFriendlyTitles();
    }
    else if ((GET_MEMBER_NAME_CHECKED(ThisClass, Effects)) == PropertyName)
    {
        UpdateEffectEditorFriendlyTitles();
    }
    else if ((GET_MEMBER_NAME_CHECKED(ThisClass, AttributeSets)) == PropertyName)
    {
        UpdateAttributeSetEditorFriendlyTitles();
    }
    else if ((GET_MEMBER_NAME_CHECKED(ThisClass, AttributeValues)) == PropertyName)
    {
        UpdateAttributeValueEditorFriendlyTitles();
    }
}

void UAeonAbilitySet::PostLoad()
{
    Super::PostLoad();
    UpdateAbilityEditorFriendlyTitles();
    UpdateEffectEditorFriendlyTitles();
    UpdateAttributeSetEditorFriendlyTitles();
    UpdateAttributeValueEditorFriendlyTitles();
}
#endif
