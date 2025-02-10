#include "Inventory/InventoryItemInstance.h"
#include "ActionGameTypes.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

// ReSharper disable once CppMemberFunctionMayBeStatic
void UInventoryItemInstance::OnRep_Equipped() {}

void UInventoryItemInstance::Init(const TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    ItemStaticDataClass = InItemStaticDataClass;
}

UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
    return ItemStaticDataClass.GetDefaultObject();
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
    DOREPLIFETIME(UInventoryItemInstance, bEquipped);
    DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

void UInventoryItemInstance::OnEquipped(AActor* ItemOwner)
{
    checkf(IsValid(ItemOwner), TEXT("UInventoryItemInstance::OnEquipped called with invalid ItemOwner"));
    const auto World = ItemOwner->GetWorld();
    // It is unclear if this scenario can arise but if it does fail out so we can fix it.
    checkf(IsValid(World),
           TEXT("UInventoryItemInstance::OnEquipped called with ItemOwner that references invalid World"));

    const FTransform Transform{};
    const auto ItemStaticData = GetItemStaticData();
    check(ItemStaticData);
    checkf(IsValid(ItemStaticData->ItemActorClass),
           TEXT("UInventoryItemInstance::OnEquipped called when static with ItemOwner that references invalid World"));

    if (!IsValid(ItemStaticData->ItemActorClass))
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryItemInstance::OnEquipped but the associated "
                  "ItemStaticData.ItemActorClass is not set. ItemStaticData={ItemStaticData}",
                  ItemStaticData->GetName());
        return;
    }

    ItemActor = World->SpawnActorDeferred<AItemActor>(ItemStaticData->ItemActorClass, Transform, ItemOwner);
    if (!IsValid(ItemActor))
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryItemInstance::OnEquipped failed to spawn actor for Item of type {ItemActorClass"
                  "}. Invoked on InventoryItemInstance {InventoryItemInstance}",
                  ItemStaticData->ItemActorClass->GetName(),
                  GetName());
        return;
    }
    ItemActor->Init(this);
    ItemActor->OnEquipped();

    // This needs to be called to finish deferred spawning and will
    // call the construction script
    ItemActor->FinishSpawning(Transform);

    // We now need to attach it to the character
    if (const auto Character = Cast<ACharacter>(ItemOwner))
    {
        if (const auto Parent = Character->GetMesh())
        {
            ItemActor->AttachToComponent(Parent,
                                         FAttachmentTransformRules::SnapToTargetIncludingScale,
                                         ItemStaticData->AttachmentSocket);
        }
    }

    bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped(AActor* ItemOwner)
{
    if (IsValid(ItemActor))
    {
        ItemActor->Destroy();
        ItemActor = nullptr;
    }

    bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* ItemOwner)
{
    if (IsValid(ItemActor))
    {
        ItemActor->OnDropped();
    }

    bEquipped = false;
}
