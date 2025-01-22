#include "AbilitySystem/Attributes/AG_AttributeSetBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

void UAG_AttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // Make sure we clamp attributes to valid ranges
    if (GetHealthAttribute() == Attribute)
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if (GetStaminaAttribute() == Attribute)
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
    }
    else if (GetMaxHealthAttribute() == Attribute || GetMaxStaminaAttribute() == Attribute)
    {
        NewValue = FMath::Max(NewValue, 1.f);
    }
}

void UAG_AttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute,
                                               const float OldValue,
                                               const float NewValue)
{
    if (GetMaxMovementSpeedAttribute() == Attribute)
    {
        if (const auto Character = Cast<ACharacter>(GetOwningActor()))
        {
            if (const auto CharacterMovementComponent = Character->GetCharacterMovement())
            {
                CharacterMovementComponent->MaxWalkSpeed = GetMaxMovementSpeed();
            }
            else
            {
                UE_LOG(LogTemp,
                       Error,
                       TEXT("Failed to set MaxMovementSpeed on Character %s. "
                            "Missing valid CharacterMovementComponent."),
                       *GetOwningActor()->GetActorNameOrLabel())
            }
        }
        else
        {
            UE_LOG(LogTemp,
                   Error,
                   TEXT("Failed to set MaxMovementSpeed on Actor %s. "
                        "Expected actor to be a Character."),
                   *GetOwningActor()->GetActorNameOrLabel())
        }
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UAG_AttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase, Health, OldHealth)
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UAG_AttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase, MaxHealth, OldMaxHealth)
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UAG_AttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase, Stamina, OldStamina)
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UAG_AttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase, MaxStamina, OldMaxStamina)
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UAG_AttributeSetBase::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAG_AttributeSetBase, MaxMovementSpeed, OldMaxMovementSpeed)
}

void UAG_AttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // No condition == send anytime it changes
    DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase, Health, COND_None, REPNOTIFY_Always)
    DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always)
    DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase, Stamina, COND_None, REPNOTIFY_Always)
    DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always)
    DOREPLIFETIME_CONDITION_NOTIFY(UAG_AttributeSetBase, MaxMovementSpeed, COND_None, REPNOTIFY_Always)
}
