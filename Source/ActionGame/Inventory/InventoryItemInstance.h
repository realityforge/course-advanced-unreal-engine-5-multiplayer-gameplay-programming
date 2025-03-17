#pragma once

#include "CoreMinimal.h"
#include "InventoryItemInstance.generated.h"

struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;
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

    void TryGrantAbilities(AActor* ItemOwner);
    void TryRemoveAbilities(AActor* ItemOwner);

    void TryApplyEffects(AActor* ItemOwner);
    void TryRemoveEffects(AActor* ItemOwner);

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

    UPROPERTY()
    TArray<FActiveGameplayEffectHandle> OngoingEffectHandles;

public:
    UPROPERTY(VisibleInstanceOnly, Replicated)
    TSubclassOf<UItemStaticData> ItemStaticDataClass{ nullptr };

    UPROPERTY(Replicated)
    int32 Quantity{ 1 };

    virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass, int32 InQuantity = 1);

    virtual bool IsSupportedForNetworking() const override { return true; }

    UItemStaticData* GetItemStaticData() const;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void OnEquipped(AActor* ItemOwner = nullptr);
    virtual void OnUnequipped(AActor* ItemOwner = nullptr);
    virtual void OnDropped(AActor* ItemOwner = nullptr);

    FORCEINLINE AItemActor* GetItemActor() const { return ItemActor; }
    FORCEINLINE int32 GetQuantity() const { return Quantity; }

    void AddItems(int32 Count);
};
