#pragma once

#include "CoreMinimal.h"
#include "InventoryItemInstance.generated.h"

class AItemActor;
class UItemStaticData;

UCLASS(BlueprintType, Blueprintable)
class ACTIONGAME_API UInventoryItemInstance : public UObject
{
    GENERATED_BODY()

    UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_Equipped)
    bool bEquipped{ false };

    /** The actor that is "currently" representing item when equipped. */
    UPROPERTY(Replicated)
    TObjectPtr<AItemActor> ItemActor{ nullptr };

    UFUNCTION()
    void OnRep_Equipped();

public:
    UPROPERTY(VisibleInstanceOnly, Replicated)
    TSubclassOf<UItemStaticData> ItemStaticDataClass{ nullptr };

    virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    virtual bool IsSupportedForNetworking() const override { return true; }

    UItemStaticData* GetItemStaticData() const;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void OnEquipped(AActor* ItemOwner = nullptr);
    virtual void OnUnequipped(AActor* ItemOwner = nullptr);
    virtual void OnDropped(AActor* ItemOwner = nullptr);
};
