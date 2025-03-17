#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "FastArrayTagCounter.h"
#include "InventoryList.h"
#include "InventoryComponent.generated.h"

struct FGameplayEventData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = false, BlueprintBaseOnly = true))
    TArray<TSubclassOf<UItemStaticData>> DefaultItems{};

    UPROPERTY(VisibleInstanceOnly, Replicated)
    TObjectPtr<UInventoryItemInstance> CurrentItem{ nullptr };

    /**
     * A map between an inventory tag and an amount.
     * This is effectively a cache that could be derived from the InventoryList but is easier to manage.
     * The key for the cache is determined by InventoryTags on ItemStaticData.
     */
    UPROPERTY(Replicated)
    FFastArrayTagCounter InventoryTags;

    TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag Tag);

public:
    // Sets default values for this component's properties
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    FInventoryList InventoryList;

    void HandleGameplayEventInternal(const FGameplayEventData& Payload);

    UFUNCTION(Server, Reliable)
    void ServerHandleGameplayEvent(FGameplayEventData Payload);

public:
    virtual void InitializeComponent() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    virtual void
    TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void RemoveItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable)
    void RemoveItemWithInventoryTag(FGameplayTag Tag, int32 Count = 1);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetInventoryTagCount(FGameplayTag Tag) const;

    UFUNCTION(BlueprintCallable)
    void AddInventoryTagCount(FGameplayTag Tag, int32 Count = 1);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItemInstance* GetCurrentItem() const;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void EquipItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void UnequipItem();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void DropItem();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void EquipNext();

    virtual void GameplayEventCallback(const FGameplayEventData* Payload);
};
