#pragma once

#include "AbilitySystem/Abilities/GameplayAbility_InventoryAbility.h"
#include "CoreMinimal.h"
#include "GameplayAbility_CombatAbility.generated.h"

UCLASS(Abstract)
class ACTIONGAME_API UGameplayAbility_CombatAbility : public UGameplayAbility_InventoryAbility
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "AbilitySystem")
    FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult) const;
};
