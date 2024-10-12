#include "AbilitySystem/Abilities/GA_Crouch.h"
#include "GameFramework/Character.h"

UGA_Crouch::UGA_Crouch()
{
    // Part of this ability runs predictively on the local client if there is one
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    // Each actor gets their own instance of this ability. State can be saved, replication is possible.
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        if (const auto Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed))
        {
            return Character->CanCrouch();
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        if (CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            const auto Actor = ActorInfo->AvatarActor.Get();
            if (const auto Character = CastChecked<ACharacter>(Actor, ECastCheckedType::NullAllowed))
            {
                Character->Crouch();
                Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
            }
        }
    }
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled)
{
    const auto Actor = ActorInfo->AvatarActor.Get();
    if (const auto Character = CastChecked<ACharacter>(Actor, ECastCheckedType::NullAllowed))
    {
        // This is a request to uncrouch and does not necessarily uncrouch
        // (Imagine the character is in a confined space and can not uncrouch)
        // To determine crouching state see Character class
        Character->UnCrouch();
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
