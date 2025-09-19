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

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "ModularGasCharacter.generated.h"

/**
 * An enum that controls which object owns the AbilityStateComponent.
 */
UENUM(BlueprintType)
enum class EAbilitySystemComponentOwnerPolicy : uint8
{
    /** The Character actor creates and owns the AbilitySystemComponent. */
    CharacterOwned,
    /** The PlayerState actor creates and owns the AbilitySystemComponent. */
    PlayerStateOwned
};

/**
 * An enum that controls which object is passed to the AbilitySystemComponent::InitAbilityActorInfo(...) method as
 * the owner and when this method is called.
 */
UENUM(BlueprintType)
enum class EAbilitySystemComponentSetupPolicy : uint8
{
    /** InitAbilityActorInfo is invoked in BeginPlay on both the Client and the Server. */
    BeginPlay,
    /**
     * InitAbilityActorInfo is invoked after the Character is possessed.
     *
     * On the Server: In APawn::PossessedBy()
     * On the Client:
     *    - APlayerController::AcknowledgePossession(...) if the Character owns the AbilitySystemComponent.
     *    - APawn::OnRep_PlayerState(...) if the PlayerState owns the AbilitySystemComponent.
     *
     * TODO: APlayerController::AcknowledgePossession not yet import into Aeon...
     */
    OnPossess,
    /** Derived classes explicitly invoke InitAbilityActorInfo. */
    Manual
};

/** Minimal class that is GAS enabled and supports extension by game feature plugins */
UCLASS(Abstract, Blueprintable)
class MODULARGASGAMEPLAYACTORS_API AModularGasCharacter : public AModularCharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

    /**
     * The policy that determines which party owns the AbilitySystemComponent.
     *
     * This determines which object is specified as the Owner when invoking
     */
    UPROPERTY(EditDefaultsOnly,
              BlueprintReadOnly,
              Category = "Aeon|AbilitySystem",
              meta = (AllowPrivateAccess = "true"))
    EAbilitySystemComponentOwnerPolicy OwnerPolicy = { EAbilitySystemComponentOwnerPolicy::CharacterOwned };

    /**
     * The policy that determines when the AbilitySystemComponent is setup.
     *
     * This is typically set to BeginPlay for AI-controlled characters and OnPossess for Player-controlled characters.
     */
    UPROPERTY(EditDefaultsOnly,
              BlueprintReadOnly,
              Category = "Aeon|AbilitySystem",
              meta = (AllowPrivateAccess = "true"))
    EAbilitySystemComponentSetupPolicy SetupPolicy{ EAbilitySystemComponentSetupPolicy::BeginPlay };

    /** The Ability system associated with the character. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aeon|AbilitySystem", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{ nullptr };

protected:
    /**
     * Sets the policy that determines which party owns the AbilitySystemComponent.
     * This should be invoked from the Constructor and nowhere else.
     *
     * @param InOwnerPolicy The policy that determines which party owns the AbilitySystemComponent.
     */
    FORCEINLINE void SetOwnerPolicy(const EAbilitySystemComponentOwnerPolicy InOwnerPolicy)
    {
        OwnerPolicy = InOwnerPolicy;
    }

    /**
     * Sets the policy that determines when the AbilitySystemComponent is initialized.
     * This should be invoked from the Constructor and nowhere else.
     *
     * This method is used to configure the setup behavior of the AbilitySystemComponent,
     * which can vary depending on whether the character is AI-controlled or Player-controlled.
     *
     * @param InSetupPolicy The policy specifying the timing of the AbilitySystemComponent setup.
     */
    FORCEINLINE void SetSetupPolicy(const EAbilitySystemComponentSetupPolicy InSetupPolicy)
    {
        SetupPolicy = InSetupPolicy;
    }

    virtual void InitAbilityActorInfo();

    virtual void ConfigureAbilitySystemComponent();

public:
    explicit AModularGasCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * The name of the AbilitySystemComponent component.
     * This is only relevant when the AbilitySystemComponentOwnerPolicy is CharacterOwned.
     */
    static const FName NAME_AbilitySystemComponent;

#pragma region APawn
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
#pragma endregion

#pragma region IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion

    FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponentFast() const { return AbilitySystemComponent; }
};
