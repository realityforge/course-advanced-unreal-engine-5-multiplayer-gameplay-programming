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

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AeonAbilityTagRelationshipMapping.generated.h"

/**
 * Rule defining additional tags for abilities with the specified tag.
 */
USTRUCT()
struct FAeonAbilityTagRelationship
{
    GENERATED_BODY()

    /** The tag that identifies the Ability this rule applies to. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTag AbilityTag;

    /** Additional AbilityTagsToBlock for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer AbilityTagsToBlock;

    /** Additional AbilityTagsToCancel for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer AbilityTagsToCancel;

    /** Additional ActivationRequiredTags for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer ActivationRequiredTags;

    /** Additional ActivationBlockedTags for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer ActivationBlockedTags;

    /** Additional SourceRequiredTags for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer SourceRequiredTags;

    /** Additional SourceBlockedTags for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer SourceBlockedTags;

    /** Additional TargetRequiredTags for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer TargetRequiredTags;

    /** Additional TargetBlockedTags for the ability. */
    UPROPERTY(EditDefaultsOnly, Category = "Ability")
    FGameplayTagContainer TargetBlockedTags;
};

/**
 * A database of rules that expand the tags of ability tags.
 *
 * When determining the block or cancel tags of an ability or the required and blocked activation tags,
 * the UAeonAbilitySystemComponent will query this class to enhance it with the declared relationships.
 * (This class just defines the rules but relies on UAeonAbilitySystemComponent to implement the rules.)
 *
 * This class exists because the default rules use exact matching when Blocking/Cancelling etc. So adding a new
 * ability that should be blocked or canceled by existing abilities would involve updating every existing ability.
 * This class offers a more scalable solution to defining rules between tags that does not
 *
 * NOTE: This is heavily inspired by the class ULyraAbilityTagRelationshipMapping.
 */
UCLASS(AutoExpandCategories = ("Ability"), Const)
class UAeonAbilityTagRelationshipMapping final : public UDataAsset
{
    GENERATED_BODY()

    /**
     * The relationship rules.
     *
     * NOTE: The database is currently an array, but perhaps we should build a cached map on loading to speed up access.
     * This will only be required if there are massive numbers of rules.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (TitleProperty = "AbilityTag"))
    TArray<FAeonAbilityTagRelationship> AbilityTagRelationships;

public:
    /**
     * Collection additional AbilityTagsToBlock and AbilityTagsToCancel given a set of ability tags.
     *
     * @param  AbilityTags The Ability Tags to lookup.
     * @param  OutAbilityTagsToBlock The container in which to add additional AbilityTagsToBlock tags.
     * @param  OutAbilityTagsToCancel The container in which to add additional AbilityTagsToCancel tags.
     */
    void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
                                        FGameplayTagContainer& OutAbilityTagsToBlock,
                                        FGameplayTagContainer& OutAbilityTagsToCancel) const;

    /**
     * Collect additional ActivationRequired and ActivationBlocked tags given a set of ability tags.
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
};
