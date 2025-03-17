#include "Inventory/InventoryList.h"
#include "ActionGameStatics.h"
#include "ActionGameTypes.h"
#include "InventoryItemInstance.h"

void FInventoryList::AddItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass)
{
    const auto DefaultQuantity = UActionGameStatics::GetItemStaticData(InItemStaticDataClass)->MaxStackCount;
    const auto ItemInstance = NewObject<UInventoryItemInstance>();
    ItemInstance->Init(InItemStaticDataClass, DefaultQuantity);
    AddItem(ItemInstance);
}

void FInventoryList::AddItem(UInventoryItemInstance* InItemInstance)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = InItemInstance;

    // This must be called if you add an item to the array
    MarkItemDirty(Item);
}

void FInventoryList::RemoveItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass)
{
    for (auto ItemIt = Items.CreateIterator(); ItemIt; ++ItemIt)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& Item = *ItemIt;
        if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass))
        {
            ItemIt.RemoveCurrent();

            // This must be called if you remove an item from the array
            MarkArrayDirty();
            break;
        }
    }
}

void FInventoryList::RemoveItem(UInventoryItemInstance* InItemInstance)
{
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance == InItemInstance)
        {
            ItemIter.RemoveCurrent();
            MarkArrayDirty();
            return;
        }
    }
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllInstancesWithTag(const FGameplayTag Tag)
{
    TArray<UInventoryItemInstance*> OutInstances;
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        auto& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->InventoryTags.Contains(Tag))
        {
            OutInstances.Add(Item.ItemInstance);
        }
    }
    return OutInstances;
}

TArray<UInventoryItemInstance*>
FInventoryList::GetAllAvailableInstancesOfType(const TSubclassOf<UItemStaticData>& ItemStaticDataClass)
{
    TArray<UInventoryItemInstance*> OutInstances;
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        auto& Item = *ItemIter;
        if (Item.ItemInstance)
        {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto Data = Item.ItemInstance->GetItemStaticData();
            if (Data->IsA(ItemStaticDataClass) && Data->MaxStackCount > Item.ItemInstance->GetQuantity())
            {
                OutInstances.Add(Item.ItemInstance);
            }
        }
    }
    return OutInstances;
}
