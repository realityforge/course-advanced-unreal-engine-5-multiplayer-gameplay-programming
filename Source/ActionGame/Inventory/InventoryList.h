#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

class UItemStaticData;
class UInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UInventoryItemInstance> ItemInstance{ nullptr };

    FORCEINLINE UInventoryItemInstance* GetItemStaticData() const { return ItemInstance; }
};

USTRUCT(BlueprintType)
struct FInventoryList : public FFastArraySerializer
{
    GENERATED_BODY()

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
    {
        return FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>(Items, DeltaParams, *this);
    }

    void AddItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass);

    void RemoveItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass);

    const TArray<FInventoryListItem>& GetItems() const { return Items; }

private:
    UPROPERTY(VisibleInstanceOnly)
    TArray<FInventoryListItem> Items;
};

template <>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};
