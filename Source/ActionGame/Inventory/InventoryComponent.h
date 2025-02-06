#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "InventoryList.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = false, BlueprintBaseOnly = true))
    TArray<TSubclassOf<UItemStaticData>> DefaultItems{};

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
};
