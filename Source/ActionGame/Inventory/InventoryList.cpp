#include "Inventory/InventoryList.h"
#include "ActionGameTypes.h"
#include "InventoryItemInstance.h"

void FInventoryList::AddItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = NewObject<UInventoryItemInstance>();
    Item.ItemInstance->Init(InItemStaticDataClass);

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
