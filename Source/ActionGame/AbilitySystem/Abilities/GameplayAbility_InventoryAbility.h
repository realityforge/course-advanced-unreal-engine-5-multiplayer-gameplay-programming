#pragma once

#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "CoreMinimal.h"
#include "GameplayAbility_InventoryAbility.generated.h"

class UInventoryItemInstance;
class UItemStaticData;
class UWeaponStaticData;
class AItemActor;
class AWeaponItemActor;
class UInventoryComponent;

UCLASS(Abstract)
class ACTIONGAME_API UGameplayAbility_InventoryAbility : public UAG_GameplayAbility
{
    GENERATED_BODY()

    /** Cached copy of Inventory Component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UInventoryComponent> InventoryComponent{ nullptr };

public:
    UGameplayAbility_InventoryAbility();

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryComponent* GetInventoryComponent() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItemInstance* GetEquippedItemInstance() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UItemStaticData* GetEquippedItemStaticData() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UWeaponStaticData* GetEquippedItemWeaponStaticData() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    AItemActor* GetEquippedItemActor() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    AWeaponItemActor* GetEquippedWeaponItemActor() const;
};
