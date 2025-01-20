/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
#include "GroupAbilitySystemComponentRegistry.generated.h"

class UGameplayAbility;
class UGameplayEffect;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;

/**
 * Registry containing a mapping between the ASC and the ability spec handles for a specific ability.
 */
USTRUCT()
struct FGroupAbilityRegistry
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TObjectPtr<UAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

    void AddAbilityToComponent(const TSubclassOf<UGameplayAbility>& Ability,
                               UAbilitySystemComponent* AbilitySystemComponent);
    void RemoveAbilityFromComponent(UAbilitySystemComponent* AbilitySystemComponent);
    void RemoveAbilityFromAllComponents();
};

/**
 * Registry containing a mapping between the ASC and the effect handles for a specific effect.
 */
USTRUCT()
struct FGroupEffectRegistry
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TObjectPtr<UAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

    void AddEffectToComponent(const TSubclassOf<UGameplayEffect>& Effect,
                              UAbilitySystemComponent* AbilitySystemComponent,
                              const float EffectLevel = 1);
    void RemoveEffectFromComponent(UAbilitySystemComponent* AbilitySystemComponent);
    void RemoveEffectFromAllComponents();
};

/**
 * Registry for managing Ability System Components (ASCs) grouped together, along with abilities and effects
 * associated with the group. This class supports hierarchical grouping, allowing inheritance of ASCs, abilities,
 * and effects from a parent group.
 */
UCLASS()
class UGroupAbilitySystemComponentRegistry : public UObject
{
    GENERATED_BODY()

    /** The parent groups registry if any. */
    UPROPERTY()
    TObjectPtr<UGroupAbilitySystemComponentRegistry> Parent{ nullptr };

    /** Ability System Components that have been explicitly registered with the group */
    UPROPERTY()
    TArray<TObjectPtr<UAbilitySystemComponent>> AbilitySystemComponents;

    /** Ability System Components that have been implicitly registered with the group by registering with a child group
     */
    UPROPERTY()
    TArray<TObjectPtr<UAbilitySystemComponent>> ImplicitAbilitySystemComponents;

    /** A map of abilities applied globally. */
    UPROPERTY()
    TMap<TSubclassOf<UGameplayAbility>, FGroupAbilityRegistry> Abilities;

    /** A map of effects applied globally. */
    UPROPERTY()
    TMap<TSubclassOf<UGameplayEffect>, FGroupEffectRegistry> Effects;

public:
    /**
     * Record the parent registry.
     *
     * @param InParent the parent registry.
     */
    void SetParent(UGroupAbilitySystemComponentRegistry* InParent);

    /** Return true if the ASC is registered. */
    bool IsAbilitySystemComponentRegistered(UAbilitySystemComponent* AbilitySystemComponent,
                                            const bool bImplicit = false) const;

    /** Register an ASC with the group and apply any active group abilities and/or effects. */
    void RegisterAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent, const bool bImplicit = false);

    /** Remove an ASC from the group, deactivate any active group abilities and/or effects. */
    void UnregisterAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent,
                                          const bool bImplicit = false);

    /**
     * Test if the specified ability has been added to the group.
     *
     * @param Ability the Ability to test
     * @return true if the ability is applied to group.
     */
    bool IsAbilityPresent(const TSubclassOf<UGameplayAbility>& Ability) const;

    /**
     * Add the specified ability to the group.
     * If any ASCs are registered with the group, then apply the ability to the ASC.
     *
     * @param Ability the Ability to add
     */
    void AddAbility(const TSubclassOf<UGameplayAbility>& Ability);

    /**
     * Remove the specified ability from the group.
     * If any ASCs are registered with the group, then remove the ability from the ASC.
     *
     * @param Ability the Ability to remove
     */
    void RemoveAbility(const TSubclassOf<UGameplayAbility>& Ability);

    /**
     * Test if the specified effect has been added to the group.
     *
     * @param Effect the Effect to test
     * @return true if the effect has been added to the group.
     */
    bool IsEffectPresent(const TSubclassOf<UGameplayEffect>& Effect) const;

    /**
     * Add the specified effect to the group.
     * If any ASCs are registered with the group, then apply the effect to the ASC.
     *
     * @param Effect the Effect to add
     */
    void AddEffect(const TSubclassOf<UGameplayEffect>& Effect);

    /**
     * Remove the specified effect from the group.
     * If any ASCs are registered with the group, then remove the effect from the ASC.
     *
     * @param Effect the Effect to remove
     */
    void RemoveEffect(const TSubclassOf<UGameplayEffect>& Effect);
};
