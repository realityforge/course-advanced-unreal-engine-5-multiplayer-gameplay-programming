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

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "AeonGamePhaseSubsystem.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

/**
 * Subsystem for managing GamePhases using Gameplay Tags.
 *
 * Game phases are represented using nested gameplay tags that allow parent and child phases to be active
 * simultaneously but not sibling phases. When a new phase is started, any active phases that are not ancestors
 * will end.
 *
 * Optionally associated with every game phase is a GameplayAbility that is given to the current GameState and activated
 * while the GamePhase is active and deactivated after the phase is deactivated.
 *
 * Example:
 *
 * Game.Playing and Game.Playing.WarmUp can be active simultaneously, but if Game.Playing.DeathMatch is made active,
 * then Game.Playing.WarmUp will be deactivated while Game.Playing will remain active. If Game.ShowingScore is
 * subsequently made active then Game.Playing and Game.Playing.DeathMatch will be deactivated.
 */
UCLASS()
class UAeonGamePhaseSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

    /** The current GamePhase. */
    UPROPERTY()
    FGameplayTag ActivePhaseTag;

    /** A map between a game phase tag and an ability that is activated on a GameState when the phase is active. */
    UPROPERTY()
    TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> PhaseAbilities;

    /** A map containing active game phase tags to handles of active phase abilities. */
    UPROPERTY()
    TMap<FGameplayTag, FGameplayAbilitySpecHandle> ActivePhases;

    bool HasNetworkAuthority() const;

    /**
     * Retrieve the AbilitySystemComponent associated with the GameState object.
     * The code will perform an ensure to verify that the component exists but may still return a nullptr.
     *
     * @return The AbilitySystemComponent on the GameState object if any.
     */
    UAbilitySystemComponent* GetGameStateAbilitySystemComponent() const;

    /**
     * Give the ability to the GameState, activate ability and record state in ActivePhases.
     *
     * This method assumes all the parameters are valid.
     *
     * @param AbilitySystemComponent The GameState AbilitySystemComponent.
     * @param PhaseTag The tag identifying the Game Phase.
     * @param Ability The ability to give to the GameState.
     */
    void ActivatePhaseAbility(UAbilitySystemComponent* AbilitySystemComponent,
                              const FGameplayTag& PhaseTag,
                              const TSubclassOf<UGameplayAbility>& Ability);

    /**
     * Cancel the ability activated on the GameState and remove state from ActivePhases.
     *
     * This method assumes all the parameters are valid.
     *
     * @param AbilitySystemComponent The GameState AbilitySystemComponent.
     * @param PhaseTag The tag identifying the Game Phase.
     */
    void DeactivatePhaseAbility(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTag PhaseTag);

protected:
    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
    /** Return true if the specified Phase has an Ability registered. */
    bool IsPhaseAbilityRegistered(const FGameplayTag& PhaseTag) const;

    /** Return the class of the registered Ability for the specified Phase. */
    TSubclassOf<UGameplayAbility>* GetPhaseAbility(const FGameplayTag& PhaseTag);

    /**
     * Register a GameplayAbility to be activated on the GameState when the game phase is active.
     * This will activate any specified ability if the phase is currently active.
     *
     * NOTE: It is expected that there is no GameplayAbility registered when this method is invoked.
     *
     * @param PhaseTag The tag identifying the Game Phase.
     * @param Ability The GameplayAbility class to register.
     */
    void RegisterPhaseAbility(const FGameplayTag& PhaseTag, const TSubclassOf<UGameplayAbility>& Ability);

    /**
     * Unregister a GameplayAbility so it will not be activated when the game phase is active.
     * This will deactivate any active abilities if the phase is currently active.
     *
     * NOTE: It is expected that there is a GameplayAbility registered when this method is invoked.
     *
     * @param PhaseTag The tag identifying the Game Phase.
     */
    void UnregisterPhaseAbility(const FGameplayTag& PhaseTag);

    /** Return true if the specified Phase or any child phase is active. */
    bool IsPhaseActive(const FGameplayTag& PhaseTag) const;
};
