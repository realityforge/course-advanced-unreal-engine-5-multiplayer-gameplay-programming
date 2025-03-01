#pragma once

#include "ActionGameTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class USphereComponent;
class UInventoryItemInstance;

UCLASS(Abstract, meta = (ShortTooltip = "An Actor that represents the item placed or spawned in the world."))
class ACTIONGAME_API AItemActor : public AActor
{
    GENERATED_BODY()

    UPROPERTY(ReplicatedUsing = OnRep_ItemInstance)
    TObjectPtr<UInventoryItemInstance> ItemInstance{ nullptr };

    UPROPERTY(ReplicatedUsing = OnRep_ItemState)
    EItemState ItemState{ EItemState::Dropped };

    UFUNCTION()
    void OnRep_ItemState();
    UFUNCTION()
    void OnRep_ItemInstance(const UInventoryItemInstance* OldItemInstance);

    UPROPERTY()
    TObjectPtr<USphereComponent> SphereComponent{ nullptr };

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UItemStaticData> ItemStaticDataClass{ nullptr };

    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex,
                                 bool bFromSweep,
                                 const FHitResult& SweepResult);

    void UpdateSphereCollision() const;

protected:
    virtual void BeginPlay() override;

    virtual void InitInternal();
public:
    AItemActor();

    void Init(UInventoryItemInstance* InItemInstance);

    virtual void OnEquipped();
    virtual void OnUnequipped();
    virtual void OnDropped();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
