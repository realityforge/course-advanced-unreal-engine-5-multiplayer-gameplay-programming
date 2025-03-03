#include "AbilitySystem/Abilities/GameplayAbility_InventoryAbility.h"
#include "ActionGameTypes.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Logging/StructuredLog.h"

void UGameplayAbility_InventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent)
    {
        UE_LOGFMT(
            LogTemp,
            Error,
            "UGameplayAbility_InventoryAbility::OnGiveAbility: Unable to locate InventoryComponent on actor {Actor}",
            GetNameSafe(ActorInfo->OwnerActor.Get()));
    }
}

UInventoryComponent* UGameplayAbility_InventoryAbility::GetInventoryComponent() const
{
    return InventoryComponent;
}

UInventoryItemInstance* UGameplayAbility_InventoryAbility::GetEquippedItemInstance() const
{
    if (InventoryComponent)
    {
        const auto InventoryItemInstance = InventoryComponent->GetCurrentItem();
        if (!InventoryItemInstance)
        {
            UE_LOGFMT(LogTemp,
                      Error,
                      "UGameplayAbility_InventoryAbility::GetEquippedItemInstance() No CurrentItem on {Actor}",
                      GetNameSafe(GetActorInfo().OwnerActor.Get()));
        }
        return InventoryItemInstance;
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UGameplayAbility_InventoryAbility::GetEquippedItemInstance() No InventoryComponent for {Actor}",
                  GetNameSafe(GetActorInfo().OwnerActor.Get()));
        return nullptr;
    }
}

const UItemStaticData* UGameplayAbility_InventoryAbility::GetEquippedItemStaticData() const
{
    const auto ItemInstance = GetEquippedItemInstance();
    return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

const UWeaponStaticData* UGameplayAbility_InventoryAbility::GetEquippedItemWeaponStaticData() const
{
    const UItemStaticData* Src = GetEquippedItemStaticData();
    return Cast<UWeaponStaticData>(Src);
}

AItemActor* UGameplayAbility_InventoryAbility::GetEquippedItemActor() const
{
    const auto ItemInstance = GetEquippedItemInstance();
    return ItemInstance ? ItemInstance->GetItemActor() : nullptr;
}

AWeaponItemActor* UGameplayAbility_InventoryAbility::GetEquippedWeaponItemActor() const
{
    AItemActor* Src = GetEquippedItemActor();
    return Cast<AWeaponItemActor>(Src);
}
