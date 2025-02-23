#include "Inventory/InventoryList.h"
#include "ActionGameTypes.h"
#include "InventoryItemInstance.h"

void FInventoryList::AddItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass)
{
    AddItem(NewObject<UInventoryItemInstance>());
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
