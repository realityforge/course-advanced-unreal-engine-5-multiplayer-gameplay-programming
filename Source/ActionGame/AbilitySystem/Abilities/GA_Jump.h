#pragma once

#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "CoreMinimal.h"
#include "GA_Jump.generated.h"

/**
 * TODO: WHy not use UGameplayAbility_CharacterJump?
 */
UCLASS()
class ACTIONGAME_API UGA_Jump : public UAG_GameplayAbility
{
    GENERATED_BODY()

    UGA_Jump();

public:
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;
};
