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
#include "Aeon/GamePhase/AeonGamePhaseSubsystem.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Aeon/Logging.h"
#include "GameFramework/GameStateBase.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonGamePhaseSubsystem)

bool UAeonGamePhaseSubsystem::HasNetworkAuthority() const
{
    const auto World = GetWorld();
    return World && NM_Client != World->GetNetMode();
}

bool UAeonGamePhaseSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return EWorldType::Game == WorldType || EWorldType::PIE == WorldType;
}

bool UAeonGamePhaseSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    const auto World = Outer->GetWorld();
    return World && NM_Client != World->GetNetMode() && Super::ShouldCreateSubsystem(Outer);
}

bool UAeonGamePhaseSubsystem::IsPhaseAbilityRegistered(const FGameplayTag& PhaseTag) const
{
    return PhaseAbilities.Contains(PhaseTag);
}

TSubclassOf<UGameplayAbility>* UAeonGamePhaseSubsystem::GetPhaseAbility(const FGameplayTag& PhaseTag)
{
    return PhaseAbilities.Find(PhaseTag);
}

void UAeonGamePhaseSubsystem::RegisterPhaseAbility(const FGameplayTag& PhaseTag,
                                                   const TSubclassOf<UGameplayAbility>& Ability)
{
    check(HasNetworkAuthority());

    if (ensureAlways(PhaseTag.IsValid()))
    {
        if (PhaseAbilities.Contains(PhaseTag))
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "RegisterPhaseAbility invoked for PhaseTag {PhaseTag} "
                      "with ability {Ability} but another ability {Existing} "
                      "is already registered for phase",
                      PhaseTag.GetTagName(),
                      Ability->GetName(),
                      PhaseAbilities.FindChecked(PhaseTag)->GetName());
        }
        else
        {
            PhaseAbilities.Add(PhaseTag, Ability);
            if (IsPhaseActive(PhaseTag))
            {
                if (const auto AbilitySystemComponent = GetGameStateAbilitySystemComponent())
                {
                    ActivatePhaseAbility(AbilitySystemComponent, PhaseTag, Ability);
                }
            }
        }
    }
    else
    {
        UE_LOGFMT(LogAeon,
                  Error,
                  "RegisterPhaseAbility invoked with invalid PhaseTag {PhaseTag}",
                  PhaseTag.GetTagName());
    }
}

void UAeonGamePhaseSubsystem::UnregisterPhaseAbility(const FGameplayTag& PhaseTag)
{
    check(HasNetworkAuthority());

    if (ensureAlwaysMsgf(PhaseTag.IsValid(),
                         TEXT("UnregisterPhaseAbility invoked with invalid PhaseTag %s"),
                         *PhaseTag.ToString()))
    {
        if (!PhaseAbilities.Contains(PhaseTag))
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "UnregisterPhaseAbility invoked for PhaseTag {PhaseTag} but no "
                      "ability is registered for phase",
                      PhaseTag.GetTagName());
        }
        else
        {
            PhaseAbilities.Remove(PhaseTag);
            if (IsPhaseActive(PhaseTag))
            {
                if (const auto AbilitySystemComponent = GetGameStateAbilitySystemComponent())
                {
                    DeactivatePhaseAbility(AbilitySystemComponent, PhaseTag);
                }
            }
        }
    }
}

void UAeonGamePhaseSubsystem::ActivatePhaseAbility(UAbilitySystemComponent* AbilitySystemComponent,
                                                   const FGameplayTag& PhaseTag,
                                                   const TSubclassOf<UGameplayAbility>& Ability)
{
    ensureAlwaysMsgf(PhaseTag.IsValid(), TEXT("PhaseTag is invalid"));

    if (ensureAlwaysMsgf(AbilitySystemComponent, TEXT("AbilitySystemComponent is null")))
    {
        FGameplayAbilitySpec Spec(Ability, 1, 0, this);
        const auto Handle = AbilitySystemComponent->GiveAbilityAndActivateOnce(Spec);

        // ReSharper disable once CppTooWideScopeInitStatement
        const auto ResultSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);

        if (ResultSpec && ResultSpec->IsActive())
        {
            ActivePhases.Add(PhaseTag, Handle);
        }
        else
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "ActivatePhaseAbility invoked with PhaseTag '{PhaseTag}' and ability '{Ability}' did not "
                      "result in an ongoing activation",
                      PhaseTag.GetTagName(),
                      Ability ? *Ability->GetName() : TEXT("?"));
        }
    }
}

void UAeonGamePhaseSubsystem::DeactivatePhaseAbility(UAbilitySystemComponent* AbilitySystemComponent,
                                                     const FGameplayTag PhaseTag)
{
    ensureAlwaysMsgf(PhaseTag.IsValid(), TEXT("PhaseTag is invalid"));

    if (ensureAlwaysMsgf(AbilitySystemComponent, TEXT("AbilitySystemComponent is null")))
    {
        if (const auto Handle = ActivePhases.Find(PhaseTag))
        {
            AbilitySystemComponent->CancelAbilityHandle(*Handle);
        }
        else
        {
            UE_LOGFMT(LogAeon,
                      Error,
                      "DeactivatePhaseAbility invoked with PhaseTag '{PhaseTag}' did not find handle in ActivePhases",
                      PhaseTag.GetTagName());
        }
    }
}

UAbilitySystemComponent* UAeonGamePhaseSubsystem::GetGameStateAbilitySystemComponent() const
{
    const auto AbilitySystemComponent = GetWorld()->GetGameState()->FindComponentByClass<UAbilitySystemComponent>();
    ensureMsgf(AbilitySystemComponent, TEXT("GameState does not have an AbilitySystemComponent"));
    return AbilitySystemComponent;
}

bool UAeonGamePhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
    return ActivePhases.Contains(PhaseTag);
}
