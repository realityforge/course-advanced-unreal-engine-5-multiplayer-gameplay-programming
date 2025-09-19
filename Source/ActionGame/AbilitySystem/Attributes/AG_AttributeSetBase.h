#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CoreMinimal.h"
#include "AG_AttributeSetBase.generated.h"

UCLASS()
class ACTIONGAME_API UAG_AttributeSetBase : public UAttributeSet
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS_BASIC(UAG_AttributeSetBase, Health);

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS_BASIC(UAG_AttributeSetBase, MaxHealth);

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS_BASIC(UAG_AttributeSetBase, Stamina);

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS_BASIC(UAG_AttributeSetBase, MaxStamina);

    UPROPERTY(BlueprintReadOnly, Category = "Movement Speed", ReplicatedUsing = OnRep_MaxMovementSpeed)
    FGameplayAttributeData MaxMovementSpeed;
    ATTRIBUTE_ACCESSORS_BASIC(UAG_AttributeSetBase, MaxMovementSpeed);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

    UFUNCTION()
    void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

    UFUNCTION()
    void OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed);
};
