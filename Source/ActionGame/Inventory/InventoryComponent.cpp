#include "Inventory/InventoryComponent.h"
#include "ActionGameTypes.h"
#include "Engine/ActorChannel.h"
#include "InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable ShowDebugInventory(TEXT("ShowDebugInventory"),
                                               0,
                                               TEXT("Draws debug info about inventory\n"
                                                    " 0 : off\n"
                                                    " 1 : on\n"),
                                               ECVF_Cheat);

UInventoryComponent::UInventoryComponent()
{
    // Should never tick in a real project but we have left it in here to help debug...
    PrimaryComponentTick.bCanEverTick = true;
    bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

void UInventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();
    if (GetOwner()->HasAuthority())
    {
        for (auto& DefaultItemClass : DefaultItems)
        {
            InventoryList.AddItem(DefaultItemClass);
        }
    }
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, InventoryList);
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bReplicatedSubobjects = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (auto& Item : InventoryList.GetItems())
    {
        if (auto ItemInstance = Item.ItemInstance; IsValid(ItemInstance))
        {
            bReplicatedSubobjects = Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
        }
    }

    return bReplicatedSubobjects;
}

void UInventoryComponent::TickComponent(float DeltaTime,
                                        ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const bool bShowDebugInventory = ShowDebugInventory->GetBool();

    // Incredibly - let's do this in tick....
    if (bShowDebugInventory)
    {
        for (auto& Item : InventoryList.GetItems())
        {
            if (auto ItemInstance = Item.ItemInstance; IsValid(ItemInstance))
            {
                const auto Data = ItemInstance->GetItemStaticData();
                if (IsValid(Data))
                {
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1,
                                                         0,
                                                         FColor::Blue,
                                                         FString::Printf(TEXT("Actor: %s Item: %s"),
                                                                         *GetOwner()->GetActorNameOrLabel(),
                                                                         *Data->ItemName.ToString()));
                    }
                }
            }
        }
    }
}
