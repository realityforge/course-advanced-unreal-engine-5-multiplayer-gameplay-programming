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
#include "Aeon/AeonFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Aeon/Logging.h"
#include "GameplayAbilitySpec.h"
#include "GameplayCueManager.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AeonFunctionLibrary)

bool UAeonFunctionLibrary::BP_TryActivateRandomSingleAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent,
                                                                  const FGameplayTag AbilityTag)
{
    return TryActivateRandomSingleAbilityByTag(AbilitySystemComponent, AbilityTag);
}

bool UAeonFunctionLibrary::TryActivateRandomSingleAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent,
                                                               const FGameplayTag AbilityTag,
                                                               FGameplayAbilitySpec** OutGameplayAbilitySpec)
{
    if (OutGameplayAbilitySpec)
    {
        *OutGameplayAbilitySpec = nullptr;
    }
    if (ensureAlways(AbilityTag.IsValid()))
    {
        if (ensureAlways(AbilitySystemComponent))
        {
            TArray<FGameplayAbilitySpec*> AbilitySpecs;
            const auto GameplayTags = AbilityTag.GetSingleTagContainer();
            AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTags, AbilitySpecs);
            if (!AbilitySpecs.IsEmpty())
            {
                const auto& AbilitySpec = AbilitySpecs[FMath::RandRange(0, AbilitySpecs.Num() - 1)];
                check(AbilitySpec);
                if (!AbilitySpec->IsActive())
                {
                    const bool bSuccess = AbilitySystemComponent->TryActivateAbility(AbilitySpec->Handle);
                    if (bSuccess && OutGameplayAbilitySpec)
                    {
                        *OutGameplayAbilitySpec = AbilitySpec;
                    }
                    return bSuccess;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                UE_LOGFMT(Aeon,
                          VeryVerbose,
                          "TryActivateRandomSingleAbilityByTag invoked with tag 'Tag' found no "
                          "matching 'Activatable' GameplayAbilitySpecs on actor 'Actor'. Either no such "
                          "Ability exists or the matching abilities are blocked or missing required tags",
                          AbilityTag.GetTagName(),
                          AbilitySystemComponent->GetOwnerActor()->GetActorNameOrLabel());
                return false;
            }
        }
        else
        {
            UE_LOGFMT(Aeon,
                      Verbose,
                      "TryActivateRandomSingleAbilityByTag invoked with tag '{Tag}' on invalid actor",
                      AbilityTag.GetTagName());
            return false;
        }
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Verbose,
                  "TryActivateRandomSingleAbilityByTag invoked with empty tag on actor '{Actor}'",
                  AbilitySystemComponent->GetOwnerActor()->GetActorNameOrLabel());
        return false;
    }
}

static UGameplayCueManager* GetGameplayCueManager()
{
    return UAbilitySystemGlobals::Get().GetGameplayCueManager();
}

void UAeonFunctionLibrary::ExecuteGameplayCueLocal(const UAbilitySystemComponent* AbilitySystemComponent,
                                                   const FGameplayTag GameplayCueTag,
                                                   const FGameplayCueParameters& GameplayCueParameters)
{
    if (AbilitySystemComponent)
    {
        GetGameplayCueManager()->HandleGameplayCue(AbilitySystemComponent->GetOwner(),
                                                   GameplayCueTag,
                                                   EGameplayCueEvent::Type::Executed,
                                                   GameplayCueParameters);
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "UAeonFunctionLibrary::ExecuteGameplayCueLocal invoked with invalid AbilitySystemComponent");
    }
}

void UAeonFunctionLibrary::AddGameplayCueLocal(const UAbilitySystemComponent* AbilitySystemComponent,
                                               const FGameplayTag GameplayCueTag,
                                               const FGameplayCueParameters& GameplayCueParameters)
{
    if (AbilitySystemComponent)
    {
        const auto GameplayCueManager = GetGameplayCueManager();
        const auto TargetActor = AbilitySystemComponent->GetOwner();
        GameplayCueManager->HandleGameplayCue(TargetActor,
                                              GameplayCueTag,
                                              EGameplayCueEvent::Type::OnActive,
                                              GameplayCueParameters);
        GameplayCueManager->HandleGameplayCue(TargetActor,
                                              GameplayCueTag,
                                              EGameplayCueEvent::Type::WhileActive,
                                              GameplayCueParameters);
    }
    else
    {
        UE_LOGFMT(Aeon, Error, "UAeonFunctionLibrary::AddGameplayCueLocal invoked with invalid AbilitySystemComponent");
    }
}

void UAeonFunctionLibrary::RemoveGameplayCueLocal(const UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayTag GameplayCueTag,
                                                  const FGameplayCueParameters& GameplayCueParameters)
{
    if (AbilitySystemComponent)
    {
        GetGameplayCueManager()->HandleGameplayCue(AbilitySystemComponent->GetOwner(),
                                                   GameplayCueTag,
                                                   EGameplayCueEvent::Type::Removed,
                                                   GameplayCueParameters);
    }
    else
    {
        UE_LOGFMT(Aeon,
                  Error,
                  "UAeonFunctionLibrary::RemoveGameplayCueLocal invoked with invalid AbilitySystemComponent");
    }
}
