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

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AeonFunctionLibrary.generated.h"

struct FGameplayAbilitySpec;
struct FGameplayCueParameters;
class UAbilitySystemComponent;
struct FGameplayTag;

/**
 * Blueprint function library exposing useful functions used within Aeon.
 */
UCLASS()
class AEON_API UAeonFunctionLibrary : public UBlueprintFunctionLibrary

{
    GENERATED_BODY()

public:
    /**
     * Activate the ability identified by the AbilityTag on specified AbilitySystemComponent.
     * - If there are multiple abilities that are identified by the tag, then an ability is randomly selected.
     * - The function will then check costs and requirements before activating the ability.
     *
     * @param AbilitySystemComponent the AbilitySystemComponent.
     * @param AbilityTag the Tag identifying the ability.
     * @return true if the Ability is present and attempt to activate occured, but it may return false positives due to
     * failure later in activation.
     */
    UFUNCTION(BlueprintCallable,
              Category = "Aeon|Ability",
              meta = (DisplayName = "Try Activate Random Single Ability By Tag", ReturnDisplayName = "Success"))
    static bool BP_TryActivateRandomSingleAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent,
                                                       const FGameplayTag AbilityTag);

    static bool TryActivateRandomSingleAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent,
                                                    const FGameplayTag AbilityTag,
                                                    FGameplayAbilitySpec** OutGameplayAbilitySpec = nullptr);

    UFUNCTION(BlueprintCallable,
              Category = "Aeon|GameplayCue",
              meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
    static void ExecuteGameplayCueLocal(const UAbilitySystemComponent* AbilitySystemComponent,
                                        const FGameplayTag GameplayCueTag,
                                        const FGameplayCueParameters& GameplayCueParameters);

    UFUNCTION(BlueprintCallable,
              Category = "Aeon|GameplayCue",
              meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
    static void AddGameplayCueLocal(const UAbilitySystemComponent* AbilitySystemComponent,
                                    const FGameplayTag GameplayCueTag,
                                    const FGameplayCueParameters& GameplayCueParameters);

    UFUNCTION(BlueprintCallable,
              Category = "Aeon|GameplayCue",
              meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
    static void RemoveGameplayCueLocal(const UAbilitySystemComponent* AbilitySystemComponent,
                                       const FGameplayTag GameplayCueTag,
                                       const FGameplayCueParameters& GameplayCueParameters);
};
