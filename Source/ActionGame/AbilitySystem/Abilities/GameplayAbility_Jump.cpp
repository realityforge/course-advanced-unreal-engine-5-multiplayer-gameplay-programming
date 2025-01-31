#include "AbilitySystem/Abilities/GameplayAbility_Jump.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGameplayAbility_Jump::UGameplayAbility_Jump()
{
    // Part of this ability runs predicatively on the local client if there is one
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayTagContainer* SourceTags,
                                               const FGameplayTagContainer* TargetTags,
                                               FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        if (const auto Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed))
        {
            const bool bMovementAllowsJump = Character->GetCharacterMovement()->IsJumpAllowed();

            const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
            const bool bIsWallRunning =
                AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(WallRunStateTag);

            return Character->CanJump() || (bMovementAllowsJump && bIsWallRunning);
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

void UGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        if (CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

            const auto Actor = ActorInfo->AvatarActor.Get();
            if (const auto Character = CastChecked<ACharacter>(Actor, ECastCheckedType::NullAllowed))
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                const auto AbilitySystemComponent =
                    UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);

                if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(WallRunStateTag))
                {
                    const FGameplayTagContainer WallRunTags(WallRunStateTag);
                    AbilitySystemComponent->CancelAbilities(&WallRunTags);
                    const auto JumpOffVector =
                        Character->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal() + FVector::UpVector;
                    Character->LaunchCharacter(JumpOffVector * OffWallJumpStrength, true, true);
                }
                else
                {
                    Character->Jump();
                }

                Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
            }
        }
    }
}
