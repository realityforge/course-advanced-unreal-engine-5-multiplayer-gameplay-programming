#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AG_CharacterMovementComponent.generated.h"

class UAeonAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class ACTIONGAME_API UAG_CharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> TraversalAbilities;

public:
    bool TryTraversal(UAeonAbilitySystemComponent* AbilitySystemComponent);
};
