#pragma once

#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "CoreMinimal.h"
#include "GameplayAbility_Jump.generated.h"

/**
 * TODO: WHy not use UGameplayAbility_CharacterJump?
 */
UCLASS(Abstract)
class ACTIONGAME_API UGameplayAbility_Jump : public UAG_GameplayAbility
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag WallRunStateTag{ FGameplayTag::EmptyTag };

    UPROPERTY(EditDefaultsOnly)
    float OffWallJumpStrength{ 100.0f };

public:
    UGameplayAbility_Jump();

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
