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
#include "Aeon/Subsystems/GroupAbilitySystem.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"
#include "Aeon/Logging.h"
#include "GroupAbilitySystemComponentRegistry.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GroupAbilitySystem)

bool UGroupAbilitySystem::HasNetworkAuthority() const
{
    const auto World = GetWorld();
    return World && NM_Client != World->GetNetMode();
}

bool UGroupAbilitySystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return EWorldType::Game == WorldType || EWorldType::PIE == WorldType;
}

bool UGroupAbilitySystem::ShouldCreateSubsystem(UObject* Outer) const
{
    const auto World = Outer->GetWorld();
    return World && NM_Client != World->GetNetMode() && Super::ShouldCreateSubsystem(Outer);
}

UGroupAbilitySystemComponentRegistry& UGroupAbilitySystem::FindOrCreateGroupRegistry(const FGameplayTag& GroupTag)
{
    check(GroupTag.IsValid());
    if (const auto Value = Registry.Find(GroupTag); Value && Value->Get())
    {
        return *Value->Get();
    }
    else
    {
        const auto ParentTag = GroupTag.RequestDirectParent();
        const auto Parent = ParentTag.IsValid() ? &FindOrCreateGroupRegistry(ParentTag) : nullptr;
        auto GroupRegistry = NewObject<UGroupAbilitySystemComponentRegistry>();
        if (nullptr != Parent)
        {
            GroupRegistry->SetParent(Parent);
        }
        Registry.Emplace(GroupTag, GroupRegistry);
        return *GroupRegistry;
    }
}

bool UGroupAbilitySystem::IsAbilitySystemComponentRegistered(const FGameplayTag GroupTag,
                                                             UAbilitySystemComponent* AbilitySystemComponent,
                                                             const bool bImplicit) const
{
    const auto Value = Registry.Find(GroupTag);
    return Value && Value->Get() ? Value->Get()->IsAbilitySystemComponentRegistered(AbilitySystemComponent, bImplicit)
                                 : false;
}

void UGroupAbilitySystem::RegisterAbilitySystemComponent(const FGameplayTag GroupTag,
                                                         UAbilitySystemComponent* AbilitySystemComponent)
{
    check(AbilitySystemComponent);
    check(HasNetworkAuthority());

    if (ensureAlways(GroupTag.IsValid()))
    {
        FindOrCreateGroupRegistry(GroupTag).RegisterAbilitySystemComponent(AbilitySystemComponent);
        UE_LOGFMT(LogAeon,
                  VeryVerbose,
                  "RegisterAbilitySystemComponent invoked with GroupTag {GroupTag}",
                  GroupTag.GetTagName());
    }
    else
    {
        UE_LOGFMT(LogAeon, Error, "RegisterAbilitySystemComponent invoked with invalid GroupTag");
    }
}

void UGroupAbilitySystem::UnregisterAbilitySystemComponent(const FGameplayTag GroupTag,
                                                           UAbilitySystemComponent* AbilitySystemComponent)
{
    check(AbilitySystemComponent);
    check(HasNetworkAuthority());

    if (ensureAlways(GroupTag.IsValid()))
    {
        FindOrCreateGroupRegistry(GroupTag).UnregisterAbilitySystemComponent(AbilitySystemComponent);
        UE_LOGFMT(LogAeon,
                  VeryVerbose,
                  "UnregisterAbilitySystemComponent invoked with GroupTag {GroupTag}",
                  GroupTag.GetTagName());
    }
    else
    {
        UE_LOGFMT(LogAeon, Error, "UnregisterAbilitySystemComponent invoked with invalid GroupTag");
    }
}

UGroupAbilitySystemComponentRegistry* UGroupAbilitySystem::FindGroupRegistry(const FGameplayTag GroupTag) const
{
    const auto Value = Registry.Find(GroupTag);
    return Value && Value->Get() ? Value->Get() : nullptr;
}

bool UGroupAbilitySystem::IsAbilityAddedToGroup(const FGameplayTag GroupTag,
                                                const TSubclassOf<UGameplayAbility> Ability) const
{
    if (ensureAlways(GroupTag.IsValid()))
    {
        const auto GroupRegistry = FindGroupRegistry(GroupTag);
        return GroupRegistry ? GroupRegistry->IsAbilityPresent(Ability) : false;
    }
    else
    {
        UE_LOGFMT(LogAeon,
                  Error,
                  "IsAbilityAddedToGroup invoked with invalid GroupTag {GroupTag}",
                  GroupTag.GetTagName());
        return false;
    }
}

void UGroupAbilitySystem::AddAbilityToGroup(const FGameplayTag GroupTag, const TSubclassOf<UGameplayAbility> Ability)
{
    if (ensureAlways(GroupTag.IsValid()))
    {
        FindOrCreateGroupRegistry(GroupTag).AddAbility(Ability);
    }
    else
    {
        UE_LOGFMT(LogAeon, Error, "AddAbilityToGroup invoked with invalid GroupTag {GroupTag}", GroupTag.GetTagName());
    }
}

void UGroupAbilitySystem::RemoveAbilityFromGroup(const FGameplayTag GroupTag,
                                                 const TSubclassOf<UGameplayAbility> Ability)
{
    if (ensureAlways(GroupTag.IsValid()))
    {
        FindOrCreateGroupRegistry(GroupTag).RemoveAbility(Ability);
    }
    else
    {
        UE_LOGFMT(LogAeon,
                  Error,
                  "RemoveAbilityFromGroup invoked with invalid GroupTag {GroupTag}",
                  GroupTag.GetTagName());
    }
}

bool UGroupAbilitySystem::IsEffectAddedToGroup(const FGameplayTag GroupTag,
                                               const TSubclassOf<UGameplayEffect> Effect) const
{
    if (ensureAlways(GroupTag.IsValid()))
    {
        const auto GroupRegistry = FindGroupRegistry(GroupTag);
        return GroupRegistry ? GroupRegistry->IsEffectPresent(Effect) : false;
    }
    else
    {
        UE_LOGFMT(LogAeon,
                  Error,
                  "IsEffectAddedToGroup invoked with invalid GroupTag {GroupTag}",
                  GroupTag.GetTagName());
        return false;
    }
}

void UGroupAbilitySystem::AddEffectToGroup(const FGameplayTag GroupTag, const TSubclassOf<UGameplayEffect> Effect)
{
    if (ensureAlways(GroupTag.IsValid()))
    {
        FindOrCreateGroupRegistry(GroupTag).AddEffect(Effect);
    }
    else
    {
        UE_LOGFMT(LogAeon, Error, "AddEffectToGroup invoked with invalid GroupTag {GroupTag}", GroupTag.GetTagName());
    }
}

void UGroupAbilitySystem::RemoveEffectFromGroup(const FGameplayTag GroupTag, const TSubclassOf<UGameplayEffect> Effect)
{
    if (ensureAlways(GroupTag.IsValid()))
    {
        FindOrCreateGroupRegistry(GroupTag).RemoveEffect(Effect);
    }
    else
    {
        UE_LOGFMT(LogAeon,
                  Error,
                  "RemoveEffectFromGroup invoked with invalid GroupTag {GroupTag}",
                  GroupTag.GetTagName());
    }
}
