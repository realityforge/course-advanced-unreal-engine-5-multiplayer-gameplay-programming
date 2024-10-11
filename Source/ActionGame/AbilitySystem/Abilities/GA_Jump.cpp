#include "AbilitySystem/Abilities/GA_Jump.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

UGA_Jump::UGA_Jump()
{
    // Part of this ability runs predictively on the local client if there is one
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    // This ability is never instanced. Anything that executes the ability is operating on the CDO.
    InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayTagContainer* SourceTags,
                                  const FGameplayTagContainer* TargetTags,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        if (const auto Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed))
        {
            return Character->CanJump();
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

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
                Character->Jump();
                Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
            }
        }
    }
}
