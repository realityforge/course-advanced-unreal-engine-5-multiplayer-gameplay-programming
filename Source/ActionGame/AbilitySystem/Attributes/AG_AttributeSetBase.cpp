#include "AbilitySystem/Attributes/AG_AttributeSetBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UAG_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (GetHealthAttribute() == Data.EvaluatedData.Attribute)
    {
        SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
    }
    else if (GetStaminaAttribute() == Data.EvaluatedData.Attribute)
    {
        SetStamina(FMath::Clamp(GetStamina(), 0, GetMaxStamina()));
    }
    else if (GetMaxMovementSpeedAttribute() == Data.EvaluatedData.Attribute)
    {
        if (const auto Character = Cast<ACharacter>(GetOwningActor()))
        {
            if (const auto CharacterMovementComponent = Character->GetCharacterMovement())
            {
                CharacterMovementComponent->MaxWalkSpeed = GetMaxMovementSpeed();
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Failed to set MaxMovementSpeed on Character %s. Missing valid CharacterMovementComponent."),
                    *GetOwningActor()->GetActorNameOrLabel())
            }
        }
        else
        {
            UE_LOG(LogTemp,
                   Warning,
                   TEXT("Failed to set MaxMovementSpeed on Actor %s. Expected actor to be a Character."),
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
