#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"

static TAutoConsoleVariable CVarShowTraversal(TEXT("ShowDebugTraversal"),
                                              0,
                                              TEXT("Draws debug info about traversal\n"
                                                   " 0 : off\n"
                                                   " 1 : on\n"),

                                              ECVF_Cheat);

bool UAG_CharacterMovementComponent::TryTraversal(UAeonAbilitySystemComponent* AbilitySystemComponent)
{
    check(AbilitySystemComponent);
    for (auto Ability : TraversalAbilities)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(Ability, true))
        {
            if (auto Spec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability))
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
