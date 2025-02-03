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
#include "GroupAbilitySystemComponentRegistry.h"
#include "AbilitySystemComponent.h"
#include "Aeon/Logging.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GroupAbilitySystemComponentRegistry)

// ------------------------------------------------------------------------------ //
//                          FGroupAbilityRegistry                                 //
// ------------------------------------------------------------------------------ //

void FGroupAbilityRegistry::AddAbilityToComponent(const TSubclassOf<UGameplayAbility>& Ability,
                                                  UAbilitySystemComponent* AbilitySystemComponent)
{
    if (Handles.Find(AbilitySystemComponent))
    {
        RemoveAbilityFromComponent(AbilitySystemComponent);
    }

    const auto CDO = Ability->GetDefaultObject<UGameplayAbility>();
    Handles.Add(AbilitySystemComponent, AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(CDO)));
}

void FGroupAbilityRegistry::RemoveAbilityFromComponent(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (const auto Handle = Handles.Find(AbilitySystemComponent))
    {
        AbilitySystemComponent->ClearAbility(*Handle);
        Handles.Remove(AbilitySystemComponent);
    }
}

void FGroupAbilityRegistry::RemoveAbilityFromAllComponents()
{
    for (const auto& Entry : Handles)
    {
        if (Entry.Key)
        {
            Entry.Key->ClearAbility(Entry.Value);
        }
    }
    Handles.Empty();
}

// ------------------------------------------------------------------------------ //
//                          FGroupEffectRegistry                                  //
// ------------------------------------------------------------------------------ //

void FGroupEffectRegistry::AddEffectToComponent(const TSubclassOf<UGameplayEffect>& Effect,
                                                UAbilitySystemComponent* AbilitySystemComponent,
                                                const float EffectLevel)
{
    if (Handles.Find(AbilitySystemComponent))
    {
        RemoveEffectFromComponent(AbilitySystemComponent);
    }

    const auto CDO = Effect->GetDefaultObject<UGameplayEffect>();
    Handles.Add(AbilitySystemComponent,
                AbilitySystemComponent->ApplyGameplayEffectToSelf(CDO,
                                                                  EffectLevel,
                                                                  AbilitySystemComponent->MakeEffectContext()));
}

void FGroupEffectRegistry::RemoveEffectFromComponent(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (const auto EffectHandle = Handles.Find(AbilitySystemComponent))
    {
        AbilitySystemComponent->RemoveActiveGameplayEffect(*EffectHandle);
        Handles.Remove(AbilitySystemComponent);
    }
}

void FGroupEffectRegistry::RemoveEffectFromAllComponents()
{
    for (const auto& Entry : Handles)
    {
        if (Entry.Key)
        {
            Entry.Key->RemoveActiveGameplayEffect(Entry.Value);
        }
    }
    Handles.Empty();
}

// ------------------------------------------------------------------------------ //
//                   FGroupAbilitySystemComponentRegistry                         //
// ------------------------------------------------------------------------------ //

void UGroupAbilitySystemComponentRegistry::SetParent(UGroupAbilitySystemComponentRegistry* InParent)
{
    check(InParent);
    check(!Parent);
    Parent = InParent;
}

bool UGroupAbilitySystemComponentRegistry::IsAbilitySystemComponentRegistered(
    UAbilitySystemComponent* AbilitySystemComponent,
    const bool bImplicit) const
{
    return (bImplicit ? ImplicitAbilitySystemComponents : AbilitySystemComponents).Contains(AbilitySystemComponent);
}

void UGroupAbilitySystemComponentRegistry::RegisterAbilitySystemComponent(
    UAbilitySystemComponent* AbilitySystemComponent,
    const bool bImplicit)
{
    check(AbilitySystemComponent);

    bool bAdded = false;
    if (bImplicit)
    {
        if (!ensureAlways(!AbilitySystemComponents.Contains(AbilitySystemComponent)))
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "RegisterAbilitySystemComponent(bImplicit=true) invoked but AbilitySystemComponent "
                      "is already explicitly registered. A child group has attempted to add the "
                      "same AbilitySystemComponent as current group");
        }
        else if (ensureAlways(!ImplicitAbilitySystemComponents.Contains(AbilitySystemComponent)))
        {
            ImplicitAbilitySystemComponents.AddUnique(AbilitySystemComponent);
            bAdded = true;
        }
        else
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "RegisterAbilitySystemComponent(bImplicit=true) invoked but AbilitySystemComponent "
                      "is already implicitly registered. Multiple child groups have attempted to add the "
                      "same AbilitySystemComponent");
        }
    }
    else
    {
        if (!ensureAlways(!ImplicitAbilitySystemComponents.Contains(AbilitySystemComponent)))
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "RegisterAbilitySystemComponent(bImplicit=false) invoked but AbilitySystemComponent "
                      "is already implicitly registered. A child group has already already registered "
                      "the same AbilitySystemComponent as the current group");
        }
        else if (ensureAlways(!AbilitySystemComponents.Contains(AbilitySystemComponent)))
        {
            AbilitySystemComponents.AddUnique(AbilitySystemComponent);
            bAdded = true;
        }
        else
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "RegisterAbilitySystemComponent(bImplicit=false) invoked but AbilitySystemComponent "
                      "is already explicitly registered. An AbilitySystemComponent can not be part of the "
                      "same group multiple times.");
        }
    }

    if (bAdded)
    {
        for (auto& Entry : Abilities)
        {
            Entry.Value.AddAbilityToComponent(Entry.Key, AbilitySystemComponent);
        }
        for (auto& Entry : Effects)
        {
            Entry.Value.AddEffectToComponent(Entry.Key, AbilitySystemComponent);
        }
        if (Parent)
        {
            Parent->RegisterAbilitySystemComponent(AbilitySystemComponent, true);
        }
    }
}

void UGroupAbilitySystemComponentRegistry::UnregisterAbilitySystemComponent(
    UAbilitySystemComponent* AbilitySystemComponent,
    const bool bImplicit)
{
    check(AbilitySystemComponent);

    bool bRemoved = false;
    if (bImplicit)
    {
        if (ensureAlways(ImplicitAbilitySystemComponents.Contains(AbilitySystemComponent)))
        {
            ImplicitAbilitySystemComponents.Remove(AbilitySystemComponent);
            bRemoved = true;
        }
        else
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "UnregisterAbilitySystemComponent(bImplicit=false) invoked but AbilitySystemComponent "
                      "is not implicitly registered.");
        }
    }
    else
    {
        if (ensureAlways(AbilitySystemComponents.Contains(AbilitySystemComponent)))
        {
            AbilitySystemComponents.Remove(AbilitySystemComponent);
            bRemoved = true;
        }
        else
        {
            UE_LOGFMT(Aeon,
                      Error,
                      "UnregisterAbilitySystemComponent(bImplicit=false) invoked but AbilitySystemComponent "
                      "is not implicitly registered.");
        }
    }

    if (bRemoved)
    {
        for (auto& EffectEntry : Effects)
        {
            EffectEntry.Value.RemoveEffectFromComponent(AbilitySystemComponent);
        }
        for (auto& AbilityEntry : Abilities)
        {
            AbilityEntry.Value.RemoveAbilityFromComponent(AbilitySystemComponent);
        }
        if (Parent)
        {
            Parent->UnregisterAbilitySystemComponent(AbilitySystemComponent, true);
        }
    }
}

bool UGroupAbilitySystemComponentRegistry::IsAbilityPresent(const TSubclassOf<UGameplayAbility>& Ability) const
{
    if (!Ability.Get())
    {
        UE_LOGFMT(Aeon, Error, "IsAbilityPresent invoked but the specified ability is null");
        return false;
    }
    else
    {
        return Abilities.Contains(Ability);
    }
}

void UGroupAbilitySystemComponentRegistry::AddAbility(const TSubclassOf<UGameplayAbility>& Ability)
{
    if (!Ability.Get())
    {
        UE_LOGFMT(Aeon, Error, "AddAbility invoked but the specified ability is null");
    }
    else if (!Abilities.Contains(Ability))
    {
        auto& Entry = Abilities.Add(Ability);
        for (const auto AbilitySystemComponent : AbilitySystemComponents)
        {
            Entry.AddAbilityToComponent(Ability, AbilitySystemComponent);
        }
        for (const auto AbilitySystemComponent : ImplicitAbilitySystemComponents)
        {
            Entry.AddAbilityToComponent(Ability, AbilitySystemComponent);
        }
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "AddAbility invoked with the ability '{Ability}' but the ability is "
                  "already added to the group",
                  GetNameSafe(Ability));
    }
}

void UGroupAbilitySystemComponentRegistry::RemoveAbility(const TSubclassOf<UGameplayAbility>& Ability)
{
    if (!Ability.Get())
    {
        UE_LOGFMT(Aeon, Error, "RemoveAbility invoked but the specified ability is null");
    }
    else if (Abilities.Contains(Ability))
    {
        Abilities[Ability].RemoveAbilityFromAllComponents();
        Abilities.Remove(Ability);
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "RemoveAbility invoked with the ability '{Ability}' but the ability is "
                  "not present in the group",
                  GetNameSafe(Ability));
    }
}

bool UGroupAbilitySystemComponentRegistry::IsEffectPresent(const TSubclassOf<UGameplayEffect>& Effect) const
{
    if (!Effect.Get())
    {
        UE_LOGFMT(Aeon, Error, "IsEffectPresent invoked but the specified effect is null");
        return false;
    }
    else
    {
        return Effects.Contains(Effect);
    }
}

void UGroupAbilitySystemComponentRegistry::AddEffect(const TSubclassOf<UGameplayEffect>& Effect)
{
    if (!Effect.Get())
    {
        UE_LOGFMT(Aeon, Error, "AddEffect invoked but the specified effect is null");
    }
    else if (!Effects.Contains(Effect))
    {
        auto& Entry = Effects.Add(Effect);
        for (const auto AbilitySystemComponent : AbilitySystemComponents)
        {
            Entry.AddEffectToComponent(Effect, AbilitySystemComponent);
        }
        for (const auto AbilitySystemComponent : ImplicitAbilitySystemComponents)
        {
            Entry.AddEffectToComponent(Effect, AbilitySystemComponent);
        }
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "AddEffect invoked with the effect '{Effect}' but the effect is already added to the group",
                  GetNameSafe(Effect));
    }
}

void UGroupAbilitySystemComponentRegistry::RemoveEffect(const TSubclassOf<UGameplayEffect>& Effect)
{
    if (!Effect.Get())
    {
        UE_LOGFMT(Aeon, Error, "RemoveEffect invoked but the specified effect is null");
    }
    else if (Effects.Contains(Effect))
    {
        Effects[Effect].RemoveEffectFromAllComponents();
        Effects.Remove(Effect);
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "RemoveEffect invoked with the effect '{Effect}' but the effect is "
                  "not present in the group",
                  GetNameSafe(Effect));
    }
}
