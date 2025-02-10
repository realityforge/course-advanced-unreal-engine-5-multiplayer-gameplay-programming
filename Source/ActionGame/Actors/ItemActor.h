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

    UPROPERTY(Replicated)
    TObjectPtr<UInventoryItemInstance> ItemInstance{ nullptr };

    // TODO: This Actor should not exist if in None state -
    //       it is either equipped or dropped ... so not sure
    //       why we have None state
    UPROPERTY(Replicated)
    EItemState ItemState{ EItemState::None };

    UPROPERTY()
    TObjectPtr<USphereComponent> SphereComponent{ nullptr };

    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex,
                                 bool bFromSweep,
                                 const FHitResult& SweepResult);

protected:
    virtual void BeginPlay() override;

public:
    AItemActor();

    void Init(UInventoryItemInstance* InItemInstance);

    virtual void OnEquipped();
    virtual void OnUnequipped();
    virtual void OnDropped();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
