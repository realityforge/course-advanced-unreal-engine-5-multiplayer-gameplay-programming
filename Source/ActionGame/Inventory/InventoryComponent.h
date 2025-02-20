#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryList.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = false, BlueprintBaseOnly = true))
    TArray<TSubclassOf<UItemStaticData>> DefaultItems{};

    UPROPERTY(Replicated)
    TObjectPtr<UInventoryItemInstance> CurrentItem{ nullptr };

public:
    // Sets default values for this component's properties
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    FInventoryList InventoryList;

public:
    virtual void InitializeComponent() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    virtual void
    TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable)
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItemInstance* GetCurrentItem() const;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void UnequipItem();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void DropItem();
};
