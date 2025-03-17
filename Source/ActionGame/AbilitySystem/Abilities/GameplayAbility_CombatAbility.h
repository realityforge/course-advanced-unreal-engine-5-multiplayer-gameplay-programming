#pragma once

#include "AbilitySystem/Abilities/GameplayAbility_InventoryAbility.h"
#include "CoreMinimal.h"
#include "GameplayAbility_CombatAbility.generated.h"

UCLASS(Abstract)
class ACTIONGAME_API UGameplayAbility_CombatAbility : public UGameplayAbility_InventoryAbility
{
    GENERATED_BODY()

protected:
    UFUNCTION(BlueprintPure)
    bool HasEnoughAmmo() const;

    UFUNCTION(BlueprintCallable)
    void DecAmmo() const;

public:
    virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle,
                               const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo,
                               FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

    UFUNCTION(BlueprintPure, Category = "AbilitySystem")
    FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult) const;

    UFUNCTION(BlueprintPure, BlueprintCallable, Category = "AbilitySystem")
    bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult) const;
};
