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
#include "Aeon/AbilitySystem/AeonAbilityTagRelationshipMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonAbilityTagRelationshipMapping)

void UAeonAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(
    const FGameplayTagContainer& AbilityTags,
    FGameplayTagContainer& OutAbilityTagsToBlock,
    FGameplayTagContainer& OutAbilityTagsToCancel) const
{
    for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& Tags = AbilityTagRelationships[i];
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            OutAbilityTagsToBlock.AppendTags(Tags.AbilityTagsToBlock);
            OutAbilityTagsToCancel.AppendTags(Tags.AbilityTagsToCancel);
        }
    }
}

void UAeonAbilityTagRelationshipMapping::GetAdditionalTagRequirements(const FGameplayTagContainer& AbilityTags,
                                                                      FGameplayTagContainer& OutActivationRequiredTags,
                                                                      FGameplayTagContainer& OutActivationBlockedTags,
                                                                      FGameplayTagContainer& OutSourceRequiredTags,
                                                                      FGameplayTagContainer& OutSourceBlockedTags,
                                                                      FGameplayTagContainer& OutTargetRequiredTags,
                                                                      FGameplayTagContainer& OutTargetBlockedTags) const
{
    for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& Tags = AbilityTagRelationships[i];
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            OutActivationRequiredTags.AppendTags(Tags.ActivationRequiredTags);
            OutActivationBlockedTags.AppendTags(Tags.ActivationBlockedTags);
            OutSourceRequiredTags.AppendTags(Tags.SourceRequiredTags);
            OutSourceBlockedTags.AppendTags(Tags.SourceBlockedTags);
            OutTargetRequiredTags.AppendTags(Tags.TargetRequiredTags);
            OutTargetBlockedTags.AppendTags(Tags.TargetBlockedTags);
        }
    }
}
