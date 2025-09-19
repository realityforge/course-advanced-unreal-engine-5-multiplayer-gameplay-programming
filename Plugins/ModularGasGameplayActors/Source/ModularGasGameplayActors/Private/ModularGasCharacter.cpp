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
#include "ModularGasCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularGasCharacter)

const FName AModularGasCharacter::NAME_AbilitySystemComponent("AbilitySystemComponent");

AModularGasCharacter::AModularGasCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // The subclass should invoke ObjectInitializer.DoNotCreateDefaultSubobject(NAME_AbilitySystemComponent) if
    // PlayerStateOwned == OwnerPolicy
    AbilitySystemComponent = CreateOptionalDefaultSubobject<UAbilitySystemComponent>(NAME_AbilitySystemComponent);
}

void AModularGasCharacter::InitAbilityActorInfo()
{
    if (ensureAlwaysMsgf(AbilitySystemComponent,
                         TEXT("On actor %s InitAbilityActorInfo() invoked but "
                              "AbilitySystemComponent not configured."),
                         *GetActorNameOrLabel()))
    {
        if (EAbilitySystemComponentOwnerPolicy::CharacterOwned == OwnerPolicy)
        {
            AbilitySystemComponent->InitAbilityActorInfo(this, this);
            ConfigureAbilitySystemComponent();
        }
        else
        {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto PS = GetPlayerState();
            if (ensureAlwaysMsgf(PS,
                                 TEXT("On actor %s the InitAbilityActorInfo() method was invoked with "
                                      "OwnerPolicy=PlayerStateOwned but PlayerState is not present. "
                                      "Is the Character Possessed?"),
                                 *GetActorNameOrLabel()))
            {
                AbilitySystemComponent->InitAbilityActorInfo(PS, this);
                ConfigureAbilitySystemComponent();
            }
        }
    }
}

void AModularGasCharacter::ConfigureAbilitySystemComponent() {}

void AModularGasCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (EAbilitySystemComponentSetupPolicy::BeginPlay == SetupPolicy)
    {
        // Init AbilityActorInfo on the Client and the Server
        InitAbilityActorInfo();
    }
}

void AModularGasCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (HasAuthority())
    {
        if (EAbilitySystemComponentSetupPolicy::OnPossess == SetupPolicy)
        {
            if (EAbilitySystemComponentOwnerPolicy::PlayerStateOwned == OwnerPolicy)
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                const auto AscPlayerState = GetPlayerState<IAbilitySystemInterface>();
                if (ensureAlwaysMsgf(
                        AscPlayerState,
                        TEXT("On actor %s SetupPolicy=OnPossess and OwnerPolicy=PlayerStateOwned "
                             "but the PlayerState object %s () does not implement IAbilitySystemInterface."),
                        *GetActorNameOrLabel(),
                        *GetNameSafe(GetPlayerState())))
                {
                    AbilitySystemComponent = AscPlayerState->GetAbilitySystemComponent();
                    check(AbilitySystemComponent);
                }
            }

            // Init AbilityActorInfo on the Server
            InitAbilityActorInfo();
        }
    }
}

void AModularGasCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    checkf(!HasAuthority(),
           TEXT("Assuming OnRep_PlayerState() is only called on client and thus ASC will never be doubly setup"));
    if (EAbilitySystemComponentSetupPolicy::OnPossess == SetupPolicy)
    {
        if (EAbilitySystemComponentOwnerPolicy::PlayerStateOwned == OwnerPolicy)
        {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto AscPlayerState = GetPlayerState<IAbilitySystemInterface>();
            if (ensureAlwaysMsgf(AscPlayerState,
                                 TEXT("On actor %s SetupPolicy=OnPossess and OwnerPolicy=PlayerStateOwned "
                                      "but the PlayerState object %s () does not implement IAbilitySystemInterface."),
                                 *GetActorNameOrLabel(),
                                 *GetNameSafe(GetPlayerState())))
            {
                AbilitySystemComponent = AscPlayerState->GetAbilitySystemComponent();
                check(AbilitySystemComponent);
            }
        }

        // Init AbilityActorInfo on the Server
        InitAbilityActorInfo();
    }
}

UAbilitySystemComponent* AModularGasCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
