#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameGamePlayTags.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Logging/StructuredLog.h"

static TAutoConsoleVariable CVarShowTraversal(TEXT("ShowDebugTraversal"),
                                              0,
                                              TEXT("Draws debug info about traversal\n"
                                                   " 0 : off\n"
                                                   " 1 : on\n"),

                                              ECVF_Cheat);

void UAG_CharacterMovementComponent::HandleMovementDirection()
{
    if (EMovementDirectionType::Strafe == MovementDirectionType)
    {
        bUseControllerDesiredRotation = true;
        bOrientRotationToMovement = false;
        CharacterOwner->bUseControllerRotationYaw = true;
    }
    else
    {
        bUseControllerDesiredRotation = false;
        bOrientRotationToMovement = true;
        CharacterOwner->bUseControllerRotationYaw = false;
    }
}

void UAG_CharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    HandleMovementDirection();

    if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        // When Movement_Enforced_Strafe Tag is added or removed then invoke callback
        const auto& Tag = ActionGameGameplayTags::Movement_Enforced_Strafe;
        AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ThisClass::OnEnforceStrafeTagChanged);
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UAG_CharacterMovementComponent::BeginPlay() Owner {Actor} has no AbilitySystemComponent",
                  GetNameSafe(GetOwner()));
    }
}

bool UAG_CharacterMovementComponent::TryTraversal(UAeonAbilitySystemComponent* AbilitySystemComponent)
{
    check(AbilitySystemComponent);
    for (const auto Ability : TraversalAbilities)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(Ability, true))
        {
            if (const auto Spec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability))
            {
                if (Spec->IsActive())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void UAG_CharacterMovementComponent::SetMovementDirectionType(const EMovementDirectionType NewMovementDirectionType)
{
    MovementDirectionType = NewMovementDirectionType;
    HandleMovementDirection();
}

void UAG_CharacterMovementComponent::OnEnforceStrafeTagChanged([[maybe_unused]] const FGameplayTag CallbackTag,
                                                               const int32 NewCount)
{
    // Strafe if Tag is present, else OrientToMovement
    SetMovementDirectionType(NewCount ? EMovementDirectionType::Strafe : EMovementDirectionType::OrientToMovement);
}
