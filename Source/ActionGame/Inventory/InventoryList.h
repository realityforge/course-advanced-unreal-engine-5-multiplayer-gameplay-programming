#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

struct FGameplayTag;
class UItemStaticData;
class UInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<UInventoryItemInstance> ItemInstance{ nullptr };

    FORCEINLINE const UInventoryItemInstance* GetItemStaticData() const { return ItemInstance; }
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
    void AddItem(UInventoryItemInstance* InItemInstance);

    void RemoveItem(const TSubclassOf<UItemStaticData>& InItemStaticDataClass);
    void RemoveItem(UInventoryItemInstance* InItemInstance);

    const TArray<FInventoryListItem>& GetItems() const { return Items; }

    TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag Tag);

    TArray<UInventoryItemInstance*>
    GetAllAvailableInstancesOfType(const TSubclassOf<UItemStaticData>& ItemStaticDataClass);

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
