#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "ActionGameCharacter.h"
#include "ActiveGameplayEffectHandle.h"

void UAG_GameplayAbility::ApplyGameplayEffects(const TWeakObjectPtr<UAbilitySystemComponent> ASC,
                                               const FGameplayEffectContextHandle& EffectContext,
                                               const bool bSaveHandle)
{
    int Index = 0;
    for (const auto Effect : bSaveHandle ? ManagedEffects : Effects)
    {
        if (Effect.Get())
        {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto EffectSpecHandle = ASC->MakeOutgoingSpec(Effect, GetAbilityLevel(), EffectContext);
            if (EffectSpecHandle.IsValid())
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                auto EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
                if (!EffectHandle.IsValid())
                {
                    ABILITY_LOG(Log,
                                TEXT("Ability %s failed to apply the effect %s"),
                                *GetNameSafe(this),
                                *GetNameSafe(Effect));
                }
                else if (bSaveHandle)
                {
                    ActiveEffectHandles.Add(EffectHandle);
                }
            }
            else
            {
                ABILITY_LOG(Log,
                            TEXT("Ability %s failed to create outgoing spec for effect %s"),
                            *GetNameSafe(this),
                            *GetNameSafe(Effect));
            }
        }
        else
        {
            ABILITY_LOG(Log,
                        TEXT("Ability %s contained a reference to invalid effect at index %d in %s"),
                        *GetNameSafe(this),
                        Index,
                        bSaveHandle ? TEXT("ManagedEffects") : TEXT("Effects"));
        }
        Index++;
    }
}

void UAG_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    const auto ASC = ActorInfo->AbilitySystemComponent.Get();
    const auto EffectContext = ASC->MakeEffectContext();

    if (ASC)
    {
        ApplyGameplayEffects(ASC, EffectContext, false);
        if (IsInstantiated())
        {
            ApplyGameplayEffects(ASC, EffectContext, true);
        }
    }
}

void UAG_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const bool bReplicateEndAbility,
                                     const bool bWasCancelled)
{
    if (IsInstantiated())
    {
        for (auto EffectHandle : ActiveEffectHandles)
        {
            if (EffectHandle.IsValid())
            {
                ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);
            }
        }
        ActiveEffectHandles.Reset();
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AActionGameCharacter* UAG_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
    return Cast<AActionGameCharacter>(GetAvatarActorFromActorInfo());
}
