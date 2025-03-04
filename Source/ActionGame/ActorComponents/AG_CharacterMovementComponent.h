#pragma once

#include "ActionGameTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AG_CharacterMovementComponent.generated.h"

enum class EMovementDirectionType : uint8;
class UAeonAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class ACTIONGAME_API UAG_CharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> TraversalAbilities;

    UPROPERTY(EditAnywhere)
    EMovementDirectionType MovementDirectionType{ EMovementDirectionType::OrientToMovement };

    void HandleMovementDirection();

public:
    virtual void BeginPlay() override;

    bool TryTraversal(UAeonAbilitySystemComponent* AbilitySystemComponent);

    UFUNCTION(BlueprintPure, BlueprintCallable)
    EMovementDirectionType GetMovementDirectionType() const { return MovementDirectionType; }

    UFUNCTION(BlueprintCallable)
    void SetMovementDirectionType(EMovementDirectionType NewMovementDirectionType);

    UFUNCTION()
    void OnEnforceStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
