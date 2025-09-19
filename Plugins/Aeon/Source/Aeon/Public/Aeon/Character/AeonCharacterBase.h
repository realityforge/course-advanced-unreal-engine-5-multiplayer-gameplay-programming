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
#include "ModularGasCharacter.h"
#include "AeonCharacterBase.generated.h"

struct FAeonAbilitySetHandles;
class UAeonAbilitySet;

/**
 * An enum to determine whether the Configure action is synchronous or synchronous.
 */
UENUM(BlueprintType)
enum class EAbilitySetLoadPolicy : uint8
{
    /** The configure action is synchronous. */
    Sync,
    /** The configure action is asynchronous. */
    Async
};

UCLASS(Abstract, meta = (ShortTooltip = "The base character class used by Aeon."))
class AEON_API AAeonCharacterBase : public AModularGasCharacter
{
    GENERATED_BODY()

    /** Setting that controls whether the AbilitySet is loaded synchronously or asynchronously. */
    UPROPERTY(EditDefaultsOnly,
              BlueprintReadOnly,
              Category = "Aeon|AbilitySystem",
              meta = (AllowPrivateAccess = "true"))
    EAbilitySetLoadPolicy AbilitySetLoadPolicy{ EAbilitySetLoadPolicy::Sync };

    /** The AbilitySet used to configure the character. */
    UPROPERTY(EditDefaultsOnly,
              BlueprintReadOnly,
              Category = "Aeon|AbilitySystem",
              meta = (AllowPrivateAccess = "true"))
    TSoftObjectPtr<UAeonAbilitySet> AbilitySet{ nullptr };

    /** Load the AbilitySet synchronously and grant the AbilitySet to the AbilitySystem. */
    void GrantAbilitySetToAbilitySystemSync();
    /** Load the AbilitySet asynchronously and grant the AbilitySet to the AbilitySystem. */
    void GrantAbilitySetToAbilitySystemAsync();
    /** The low-level method to grant the AbilitySet to the AbilitySystem. */
    void GrantAbilitySet(const UAeonAbilitySet* Data, FAeonAbilitySetHandles* OutGrantedHandles = nullptr);

protected:
    FORCEINLINE EAbilitySetLoadPolicy GetAbilitySetLoadPolicy() const { return AbilitySetLoadPolicy; }
    FORCEINLINE void SetAbilitySetLoadPolicy(const EAbilitySetLoadPolicy InAbilitySetLoadPolicy)
    {
        AbilitySetLoadPolicy = InAbilitySetLoadPolicy;
    }

    virtual int32 GetAbilityLevel() const;

    virtual void OnAbilitySystemComponentInitialized();

#pragma region AModularGasCharacter
    virtual void ConfigureAbilitySystemComponent() override;
#pragma endregion

public:
    explicit AAeonCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
