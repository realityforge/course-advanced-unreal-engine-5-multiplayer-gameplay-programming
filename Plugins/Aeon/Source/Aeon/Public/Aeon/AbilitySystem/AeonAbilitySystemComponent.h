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

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "AeonAbilitySystemComponent.generated.h"

class UAeonAbilityTagRelationshipMapping;

/** The AbilitySystemComponent specialization used in Aeon */
UCLASS()
class AEON_API UAeonAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    /**
     * Callback invoked when an AbilityInputAction has associated Input pressed.
     *
     * @param Tag The InputTag that identifies the ability.
     * @param bLogIfUnmatched A flag that controls whether a log message is emitted if no ability matches the Tag.
     * @see UAeonInputConfig for where AbilityInputAction are defined
     */
    void OnAbilityInputPressed(const FGameplayTag& Tag, bool bLogIfUnmatched = true);

    /**
     * Callback invoked when an AbilityInputAction has associated Input held.
     *
     * @param Tag The InputTag that identifies the ability.
     * @param bLogIfUnmatched A flag that controls whether a log message is emitted if no ability matches the Tag.
     * @see UAeonInputConfig for where AbilityInputAction are defined
     */
    void OnAbilityInputHeld(const FGameplayTag& Tag, bool bLogIfUnmatched = true);

    /**
     * Callback invoked when an AbilityInputAction has associated Input released.
     *
     * @param Tag The InputTag that identifies the ability.
     * @param bLogIfUnmatched A flag that controls whether a log message is emitted if no ability matches the Tag.
     * @see UAeonInputConfig for where AbilityInputAction are defined
     */
    void OnAbilityInputReleased(const FGameplayTag& Tag, bool bLogIfUnmatched = true);

#pragma region AbilityTagRelationship Support

private:
    /**
     * An optional structure used to explicitly define relationships between tags.
     *
     * So that we can define additional activation tag requirement tags, blocking and canceling tags.
     * See @link UAeonAbilityTagRelationshipMapping @endlink for further details.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Aeon|Ability")
    TObjectPtr<UAeonAbilityTagRelationshipMapping> TagRelationshipMapping;

public:
    // --------------------------------------------------- //
    // AbilityTagRelationship Support
    // --------------------------------------------------- //

    /**
     * Set the tag relationship mapping.
     *
     * @param InTagRelationshipMapping The new tag relationship mapping. The parameter may be null.
     */
    void SetTagRelationshipMapping(UAeonAbilityTagRelationshipMapping* InTagRelationshipMapping);

    virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
                                                UGameplayAbility* RequestingAbility,
                                                bool bEnableBlockTags,
                                                const FGameplayTagContainer& BlockTags,
                                                bool bExecuteCancelTags,
                                                const FGameplayTagContainer& CancelTags) override;

    /**
     * Collect additional ActivationRequired and ActivationBlocked tags for the specified AbilityTags.
     *
     * @param  AbilityTags The Ability Tags to lookup.
     * @param  OutActivationRequiredTags The container in which to add additional ActivationRequired tags.
     * @param  OutActivationBlockedTags The container in which to add additional ActivationBlocked tags.
     * @param  OutSourceRequiredTags The container in which to add additional SourceRequired tags.
     * @param  OutSourceBlockedTags The container in which to add additional SourceBlocked tags.
     * @param  OutTargetRequiredTags The container in which to add additional TargetRequired tags.
     * @param  OutTargetBlockedTags The container in which to add additional TargetBlocked tags.
     */
    void GetAdditionalTagRequirements(const FGameplayTagContainer& AbilityTags,
                                      FGameplayTagContainer& OutActivationRequiredTags,
                                      FGameplayTagContainer& OutActivationBlockedTags,
                                      FGameplayTagContainer& OutSourceRequiredTags,
                                      FGameplayTagContainer& OutSourceBlockedTags,
                                      FGameplayTagContainer& OutTargetRequiredTags,
                                      FGameplayTagContainer& OutTargetBlockedTags) const;

#pragma endregion
};
