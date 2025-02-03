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
#include "Aeon/AbilitySystem/AeonGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonGameplayAbility)

void UAeonGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    MaybeActivateOnGivenAbility(ActorInfo, Spec);
}

bool UAeonGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                             const FGameplayTagContainer* SourceTags,
                                                             const FGameplayTagContainer* TargetTags,
                                                             FGameplayTagContainer* OptionalRelevantTags) const
{
    // NOTE: This method was overriden to incorporate support for the AbilityTagRelationship

    FGameplayTagContainer AllActivationRequiredTags = ActivationRequiredTags;
    FGameplayTagContainer AllActivationBlockedTags = ActivationBlockedTags;
    FGameplayTagContainer AllSourceRequiredTags = SourceRequiredTags;
    FGameplayTagContainer AllSourceBlockedTags = SourceBlockedTags;
    FGameplayTagContainer AllTargetRequiredTags = TargetRequiredTags;
    FGameplayTagContainer AllTargetBlockedTags = TargetBlockedTags;

    // Expand our ability tags to add additional tag requirements
    if (const auto AeonASC = Cast<UAeonAbilitySystemComponent>(&AbilitySystemComponent))
    {
        AeonASC->GetAdditionalTagRequirements(GetAssetTags(),
                                              AllActivationRequiredTags,
                                              AllActivationBlockedTags,
                                              AllSourceRequiredTags,
                                              AllSourceBlockedTags,
                                              AllTargetRequiredTags,
                                              AllTargetBlockedTags);
    }

    // The code below this point in the method is copied from the code in the base class with replacements:
    //   "ActivationRequiredTags" => "AllActivationRequiredTags"
    //   "ActivationBlockedTags" => "AllActivationBlockedTags"
    //   "SourceRequiredTags" => "AllSourceRequiredTags"
    //   "SourceBlockedTags" => "AllSourceBlockedTags"
    //   "TargetRequiredTags" => "AllTargetRequiredTags"
    //   "TargetBlockedTags" => "AllTargetBlockedTags"

    // Define a common lambda to check for blocked tags
    bool bBlocked = false;
    auto CheckForBlocked = [&](const FGameplayTagContainer& ContainerA, const FGameplayTagContainer& ContainerB) {
        // Do we not have any tags in common?  Then we're not blocked
        if (ContainerA.IsEmpty() || ContainerB.IsEmpty() || !ContainerA.HasAny(ContainerB))
        {
            return;
        }

        if (OptionalRelevantTags)
        {
            // Ensure the global blocking tag is only added once
            if (!bBlocked)
            {
                UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
                const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
                OptionalRelevantTags->AddTag(BlockedTag);
            }

            // Now append all the blocking tags
            OptionalRelevantTags->AppendMatchingTags(ContainerA, ContainerB);
        }

        bBlocked = true;
    };

    // Define a common lambda to check for missing required tags
    bool bMissing = false;
    auto CheckForRequired = [&](const FGameplayTagContainer& TagsToCheck, const FGameplayTagContainer& RequiredTags) {
        // Do we have no requirements, or have met all requirements?  Then nothing's missing
        if (RequiredTags.IsEmpty() || TagsToCheck.HasAll(RequiredTags))
        {
            return;
        }

        if (OptionalRelevantTags)
        {
            // Ensure the global missing tag is only added once
            if (!bMissing)
            {
                UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
                const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;
                OptionalRelevantTags->AddTag(MissingTag);
            }

            FGameplayTagContainer MissingTags = RequiredTags;
            MissingTags.RemoveTags(TagsToCheck.GetGameplayTagParents());
            OptionalRelevantTags->AppendTags(MissingTags);
        }

        bMissing = true;
    };

    // Start by checking all the blocked tags first (so OptionalRelevantTags will contain blocked tags first)
    CheckForBlocked(AbilitySystemComponent.GetBlockedAbilityTags(), GetAssetTags());
    CheckForBlocked(AbilitySystemComponent.GetOwnedGameplayTags(), AllActivationBlockedTags);
    if (SourceTags)
    {
        CheckForBlocked(*SourceTags, AllSourceBlockedTags);
    }
    if (TargetTags)
    {
        CheckForBlocked(*TargetTags, AllTargetBlockedTags);
    }

    // Now check all required tags
    CheckForRequired(AbilitySystemComponent.GetOwnedGameplayTags(), AllActivationRequiredTags);
    if (SourceTags)
    {
        CheckForRequired(*SourceTags, AllSourceRequiredTags);
    }
    if (TargetTags)
    {
        CheckForRequired(*TargetTags, AllTargetRequiredTags);
    }

    // We succeeded if there were no blocked tags and no missing required tags
    return !bBlocked && !bMissing;
}

void UAeonGameplayAbility::MaybeActivateOnGivenAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilitySpec& Spec) const
{
    // The expanded logic wrt replication while not necessary now was extracted form several
    // downstream projects. Although it seems that most of these tutorials or projects, probably
    // cargo-cult copied this from Lyra or an earlier project. Duplicating this until a later date
    // at which point we can re-assess

    if (EAeonAbilityActivationPolicy::OnGiven == GetAbilityActivationPolicy() && ActorInfo && !Spec.IsActive())
    {
        if (const auto AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
        {
            if (const auto AvatarActor = ActorInfo->AvatarActor.Get())
            {
                // Actors that are no longer replicated or about to die should not attempt to activate ability
                if (!AvatarActor->GetTearOff() && AvatarActor->GetLifeSpan() <= 0.0f)
                {
                    const bool bIsLocalExecution =
                        (EGameplayAbilityNetExecutionPolicy::LocalPredicted == GetNetExecutionPolicy())
                        || (EGameplayAbilityNetExecutionPolicy::LocalOnly == GetNetExecutionPolicy());
                    const bool bIsServerExecution =
                        (EGameplayAbilityNetExecutionPolicy::ServerOnly == GetNetExecutionPolicy())
                        || (EGameplayAbilityNetExecutionPolicy::ServerInitiated == GetNetExecutionPolicy());

                    const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
                    // ReSharper disable once CppTooWideScopeInitStatement
                    const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;
                    if (bClientShouldActivate || bServerShouldActivate)
                    {
                        AbilitySystemComponent->TryActivateAbility(Spec.Handle);
                    }
                }
            }
        }
    }
}

void UAeonGameplayAbility::MaybeClearOnGivenAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo) const
{
    // If an activation policy is OnGiven then we remove it after the ability completes
    if (EAeonAbilityActivationPolicy::OnGiven == AbilityActivationPolicy)
    {
        if (ActorInfo)
        {
            ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
        }
    }
}

void UAeonGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const bool bReplicateEndAbility,
                                      const bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    MaybeClearOnGivenAbility(Handle, ActorInfo);
}

UAeonAbilitySystemComponent* UAeonGameplayAbility::GetAeonAbilitySystemComponentFromActorInfo() const
{
    return CastChecked<UAeonAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}
