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

#include "Aeon/AbilitySystem/AeonGameplayAbility.h"
#include "CoreMinimal.h"
#include "AeonGameplayAbility_Group.generated.h"

/**
 * Ability to associate the owning AbilitySystemComponent with the specified group.
 *
 * <p>The AbilitySystemComponent is registered with the group using the
 * @link UGroupAbilitySystem @endlink subsystem.</p>
 */
UCLASS(Abstract, meta = (RuleRangerDataOnly))
class AEON_API UAeonGameplayAbility_Group : public UAeonGameplayAbility
{
    GENERATED_BODY()

    /** The group to place the AbilitySystemComponent in. */
    UPROPERTY(EditDefaultsOnly, Category = "Aeon|Ability", meta = (Categories = "Group", AllowPrivateAccess = true))
    FGameplayTag GroupTag{ FGameplayTag::EmptyTag };

public:
    explicit UAeonGameplayAbility_Group(const FObjectInitializer& ObjectInitializer);

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
