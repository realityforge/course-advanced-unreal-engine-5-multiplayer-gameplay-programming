#include "AbilitySystem/Abilities/GameplayAbility_Crouch.h"
#include "GameFramework/Character.h"

UGameplayAbility_Crouch::UGameplayAbility_Crouch()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGameplayAbility_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

void UGameplayAbility_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

void UGameplayAbility_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const bool bReplicateEndAbility,
                                         const bool bWasCancelled)
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
