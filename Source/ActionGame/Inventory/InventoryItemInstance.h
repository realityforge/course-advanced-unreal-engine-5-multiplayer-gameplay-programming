#pragma once

#include "CoreMinimal.h"
#include "InventoryItemInstance.generated.h"

class UItemStaticData;

UCLASS(BlueprintType, Blueprintable)
class ACTIONGAME_API UInventoryItemInstance : public UObject
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly, Replicated)
    TSubclassOf<UItemStaticData> ItemStaticDataClass{ nullptr };

    UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_Equipped)
    bool bEquipped{ false };

    UFUNCTION()
    void OnRep_Equipped();

    virtual void OnEquipped(AActor* ItemOwner = nullptr) {}
    virtual void OnUnequipped(AActor* ItemOwner = nullptr) {}
    virtual void OnDropped(AActor* ItemOwner = nullptr) {}

public:
    virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    virtual bool IsSupportedForNetworking() const override { return true; }

    UItemStaticData* GetItemStaticData() const;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
