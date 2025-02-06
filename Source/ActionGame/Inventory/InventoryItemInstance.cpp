#include "Inventory/InventoryItemInstance.h"
#include "ActionGameTypes.h"
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
}
