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
#include "ModularGameMode.h"
#include "ModularGasGameMode.generated.h"

/** Minimal class that is GAS enabled and supports extension by game feature plugins */
UCLASS(Abstract, Blueprintable)
class MODULARGASGAMEPLAYACTORS_API AModularGasGameModeBase : public AModularGameModeBase, public IAbilitySystemInterface
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{ nullptr };

public:
    explicit AModularGasGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /** The name of the AbilitySystemComponent component */
    static const FName NAME_AbilitySystemComponent;

    //~ Begin IAbilitySystemInterface Interface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End IAbilitySystemInterface Interface

    FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponentFast() const { return AbilitySystemComponent; }
};

/** Pair this with a ModularGameState */
UCLASS(Abstract, Blueprintable)
class MODULARGASGAMEPLAYACTORS_API AModularGasGameMode : public AModularGameMode, public IAbilitySystemInterface
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent{ nullptr };

public:
    explicit AModularGasGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /** The name of the AbilitySystemComponent component */
    static const FName NAME_AbilitySystemComponent;

    //~ Begin IAbilitySystemInterface Interface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End IAbilitySystemInterface Interface

    FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponentFast() const { return AbilitySystemComponent; }
};
