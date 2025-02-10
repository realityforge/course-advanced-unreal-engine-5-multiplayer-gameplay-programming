#include "Inventory/InventoryComponent.h"
#include "ActionGameTypes.h"
#include "Engine/ActorChannel.h"
#include "InventoryItemInstance.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable CVarShowDebugInventory(TEXT("ShowDebugInventory"),
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

    DOREPLIFETIME(UInventoryComponent, CurrentItem);
    DOREPLIFETIME(UInventoryComponent, InventoryList);
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bReplicatedSubobjects = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (auto& Item : InventoryList.GetItems())
    {
        if (auto ItemInstance = Item.ItemInstance; IsValid(ItemInstance))
        {
            bReplicatedSubobjects |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
        }
    }

    return bReplicatedSubobjects;
}

void UInventoryComponent::TickComponent(float DeltaTime,
                                        ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const bool bShowDebugInventory = CVarShowDebugInventory->GetBool();

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

void UInventoryComponent::AddItem(const TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    InventoryList.AddItem(InItemStaticDataClass);
}

void UInventoryComponent::RemoveItem(const TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    InventoryList.RemoveItem(InItemStaticDataClass);
}

void UInventoryComponent::EquipItem(const TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    // Check should not be required from Blueprints but is present in case called from C++ code
    if (GetOwner()->HasAuthority())
    {
        for (const auto& Item : InventoryList.GetItems())
        {
            if (Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass)
            {
                Item.ItemInstance->OnEquipped(GetOwner());
                CurrentItem = Item.ItemInstance;
                return;
            }
        }
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryComponent::EquipItem({ItemStaticDataClass}) invoked on actor {Actor} but "
                  "no such item exists in inventory",
                  InItemStaticDataClass->GetFName(),
                  GetOwner()->GetActorNameOrLabel());
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryComponent::EquipItem({ItemStaticDataClass}) invoked on non-authoritative actor {Actor}",
                  InItemStaticDataClass->GetFName(),
                  GetOwner()->GetActorNameOrLabel());
    }
}

void UInventoryComponent::UnequipItem()
{
    // Check should not be required from Blueprints but is present in case called from C++ code
    if (GetOwner()->HasAuthority())
    {
        if (IsValid(CurrentItem))
        {
            CurrentItem->OnUnequipped(GetOwner());
            CurrentItem = nullptr;
        }
        else
        {
            UE_LOGFMT(LogTemp,
                      Warning,
                      "UInventoryComponent::UnequipItem() invoked on actor {Actor} but "
                      "no item equipped",
                      GetOwner()->GetActorNameOrLabel());
        }
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryComponent::UnequipItem() invoked on non-authoritative actor {Actor}",
                  GetOwner()->GetActorNameOrLabel());
    }
}

void UInventoryComponent::DropItem()
{
    // Check should not be required from Blueprints but is present in case called from C++ code
    if (GetOwner()->HasAuthority())
    {
        if (IsValid(CurrentItem))
        {
            RemoveItem(CurrentItem->ItemStaticDataClass);
            CurrentItem->OnDropped(GetOwner());
            CurrentItem = nullptr;
        }
        else
        {
            UE_LOGFMT(LogTemp,
                      Warning,
                      "UInventoryComponent::DropItem() invoked on actor {Actor} but "
                      "no item equipped",
                      GetOwner()->GetActorNameOrLabel());
        }
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryComponent::DropItem() invoked on non-authoritative actor {Actor}",
                  GetOwner()->GetActorNameOrLabel());
    }
}

UInventoryItemInstance* UInventoryComponent::GetCurrentItem() const
{
    return CurrentItem.Get();
}
