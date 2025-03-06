#pragma once

#include "Abilities/GameplayAbility.h"
#include "CoreMinimal.h"
#include "AG_GameplayAbility.generated.h"

class AActionGameCharacter;

UCLASS(Abstract)
class ACTIONGAME_API UAG_GameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

    /** Effects to apply on activation but not explicitly removed on ability end. */
    UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
    TArray<TSubclassOf<UGameplayEffect>> Effects;
    /** Effects to apply on activation and explicitly removed on ability end. */
    UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
    TArray<TSubclassOf<UGameplayEffect>> ManagedEffects;

    TArray<FActiveGameplayEffectHandle> ActiveEffectHandles;

    void ApplyGameplayEffects(TWeakObjectPtr<UAbilitySystemComponent> ASC,
                              const FGameplayEffectContextHandle& EffectContext,
                              bool bSaveHandle);

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Action")
    AActionGameCharacter* GetActionGameCharacterFromActorInfo() const;
};
