#include "Inventory/InventoryComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameGamePlayTags.h"
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

TArray<UInventoryItemInstance*> UInventoryComponent::GetAllInstancesWithTag(const FGameplayTag Tag)
{
    return InventoryList.GetAllInstancesWithTag(Tag);
}

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

void UInventoryComponent::HandleGameplayEventInternal(const FGameplayEventData& Payload)
{
    if (GetOwner()->HasAuthority())
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto& EventTag = Payload.EventTag;
        if (ActionGameGameplayTags::Event_Inventory_EquipItemActor.GetTag() == EventTag)
        {
            if (const auto ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
            {
                // Payload.OptionalObject has a const qualifier, const_cast to remove this qualifier
                AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));
                if (Payload.Instigator)
                {
                    // We assume that if Instigator set then it was an actor that would be picked up
                    // so we destroy the actor in the world
                    const_cast<AActor*>(Payload.Instigator.Get())->Destroy();
                }
            }
        }
        else if (ActionGameGameplayTags::Event_Inventory_DropItem.GetTag() == EventTag)
        {
            DropItem();
        }
        else if (ActionGameGameplayTags::Event_Inventory_EquipNext.GetTag() == EventTag)
        {
            EquipNext();
        }
        else if (ActionGameGameplayTags::Event_Inventory_Unequip.GetTag() == EventTag)
        {
            UnequipItem();
        }
        else
        {
            UE_LOGFMT(LogTemp,
                      Error,
                      "UInventoryComponent::HandleGameplayEventInternal(...) for Owner "
                      "{ActorName} failed to handle event tag {Tag}",
                      GetOwner()->GetActorNameOrLabel(),
                      EventTag.GetTagName());
        }
    }
}

void UInventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
    HandleGameplayEventInternal(Payload);
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
    if (UAbilitySystemComponent* AbilitySystemComponent =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        auto& Callbacks = AbilitySystemComponent->GenericGameplayEventCallbacks;
        // Route all of our callbacks through a single handler.
        // These handlers have to use exact tag matches
        Callbacks.FindOrAdd(ActionGameGameplayTags::Event_Inventory_EquipItemActor)
            .AddUObject(this, &UInventoryComponent::GameplayEventCallback);
        Callbacks.FindOrAdd(ActionGameGameplayTags::Event_Inventory_DropItem)
            .AddUObject(this, &UInventoryComponent::GameplayEventCallback);
        Callbacks.FindOrAdd(ActionGameGameplayTags::Event_Inventory_EquipNext)
            .AddUObject(this, &UInventoryComponent::GameplayEventCallback);
        Callbacks.FindOrAdd(ActionGameGameplayTags::Event_Inventory_Unequip)
            .AddUObject(this, &UInventoryComponent::GameplayEventCallback);
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

    // ReSharper disable once CppTooWideScope
    const bool bShowDebugInventory = CVarShowDebugInventory->GetBool();

    // Incredibly - let's do this in tick....
    if (bShowDebugInventory)
    {
        for (auto& Item : InventoryList.GetItems())
        {
            if (auto ItemInstance = Item.ItemInstance; IsValid(ItemInstance))
            {
                if (const auto Data = ItemInstance->GetItemStaticData(); IsValid(Data))
                {
                    if (GEngine)
                    {

                        const auto NetMode = GetWorld()->GetNetMode();
                        const auto NetModeDesc = NM_Standalone == NetMode ? TEXT("Standalone")
                            : NM_DedicatedServer == NetMode               ? TEXT("DedicatedServer")
                            : NM_ListenServer == NetMode                  ? TEXT("ListenServer")
                            : NM_Client == NetMode                        ? TEXT("Client")
                                                                          : TEXT("Unknown");
                        GEngine->AddOnScreenDebugMessage(-1,
                                                         0,
                                                         FColor::Blue,
                                                         FString::Printf(TEXT("NetMode: %s Actor: %s Item: %s"),
                                                                         NetModeDesc,
                                                                         *GetOwner()->GetActorNameOrLabel(),
                                                                         *Data->ItemName.ToString()));
                    }
                }
            }
            for (const auto& Record : InventoryTags.GetTagArray())
            {
                GEngine->AddOnScreenDebugMessage(
                    -1,
                    0,
                    FColor::Purple,
                    FString::Printf(TEXT("Tag %s count: %d"), *Record.Tag.ToString(), Record.Count));
            }
        }
    }
}

void UInventoryComponent::AddItem(const TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (GetOwner()->HasAuthority())
    {
        InventoryList.AddItem(InItemStaticDataClass);
    }
}

void UInventoryComponent::AddItemInstance(UInventoryItemInstance* InItemInstance)
{
    if (GetOwner()->HasAuthority())
    {
        auto Items = InventoryList.GetAllAvailableInstancesOfType(InItemInstance->ItemStaticDataClass);

        // Sort instances so that those that have fewer slots are first so that we will up slots
        // from the most full to the least full
        Algo::Sort(Items, [](auto InA, auto InB) { return InA->GetQuantity() < InB->GetQuantity(); });

        // Each slot will hold at most this count
        const auto MaxItemStackCount = InItemInstance->GetItemStaticData()->MaxStackCount;
        auto ItemsLeft = InItemInstance->GetQuantity();
        for (const auto Item : Items)
        {
            const auto EmptySlots = MaxItemStackCount - Item->GetQuantity();
            const auto SlotsToAdd = FMath::Max(EmptySlots, ItemsLeft);
            ItemsLeft -= SlotsToAdd;
            Item->AddItems(SlotsToAdd);
            InItemInstance->AddItems(-SlotsToAdd);
            for (const auto InventoryTag : Item->GetItemStaticData()->InventoryTags)
            {
                InventoryTags.AddTagCount(InventoryTag, SlotsToAdd);
            }

            if (ItemsLeft <= 0)
            {
                return;
            }
        }
        while (ItemsLeft > MaxItemStackCount)
        {
            AddItem(InItemInstance->GetItemStaticData()->GetClass());
            for (const auto InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
            {
                InventoryTags.AddTagCount(InventoryTag, MaxItemStackCount);
            }
            ItemsLeft -= MaxItemStackCount;
            InItemInstance->AddItems(-MaxItemStackCount);
        }

        InventoryList.AddItem(InItemInstance);
        for (const auto InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
        {
            InventoryTags.AddTagCount(InventoryTag, InItemInstance->GetQuantity());
        }
    }
}

void UInventoryComponent::RemoveItem(const TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (GetOwner()->HasAuthority())
    {
        InventoryList.RemoveItem(InItemStaticDataClass);
    }
}

void UInventoryComponent::RemoveItemInstance(UInventoryItemInstance* InItemInstance)
{
    if (GetOwner()->HasAuthority())
    {
        InventoryList.RemoveItem(InItemInstance);
    }
}

void UInventoryComponent::RemoveItemWithInventoryTag(const FGameplayTag Tag, const int32 Count)
{
    if (GetOwner()->HasAuthority())
    {
        int32 ItemsLeft = Count;
        auto Items = GetAllInstancesWithTag(Tag);

        Algo::Sort(Items, [](auto InA, auto InB) { return InA->GetQuantity() < InB->GetQuantity(); });

        for (const auto Item : Items)
        {
            const auto AvailableCount = Item->GetQuantity();
            const auto SlotsToRemove = FMath::Min(AvailableCount, ItemsLeft);
            UE_LOGFMT(LogTemp,
                      Warning,
                      "AvailableCount={AvailableCount} SlotsToRemove={SlotsToRemove}",
                      AvailableCount,
                      SlotsToRemove);
            if (SlotsToRemove >= AvailableCount)
            {
                RemoveItemInstance(Item);
            }
            else
            {
                Item->AddItems(-SlotsToRemove);
            }
            for (const auto InventoryTag : Item->GetItemStaticData()->InventoryTags)
            {
                InventoryTags.AddTagCount(InventoryTag, -SlotsToRemove);
            }
            ItemsLeft -= SlotsToRemove;
            if (ItemsLeft <= 0)
            {
                break;
            }
        }
    }
}

int32 UInventoryComponent::GetInventoryTagCount(const FGameplayTag Tag) const
{
    return InventoryTags.GetTagCount(Tag);
}

void UInventoryComponent::AddInventoryTagCount(const FGameplayTag Tag, const int32 Count)
{
    InventoryTags.AddTagCount(Tag, Count);
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

void UInventoryComponent::EquipItemInstance(UInventoryItemInstance* InItemInstance)
{
    check(InItemInstance);
    if (GetOwner()->HasAuthority())
    {
        for (const auto& Item : InventoryList.GetItems())
        {
            if (Item.ItemInstance == InItemInstance)
            {
                Item.ItemInstance->OnEquipped(GetOwner());
                CurrentItem = Item.ItemInstance;
                return;
            }
        }
    }
    UE_LOGFMT(LogTemp,
              Error,
              "UInventoryComponent::EquipItemInstance({InItemInstance}) invoked on actor {Actor} but "
              "no such item exists in inventory",
              InItemInstance->GetFName(),
              GetOwner()->GetActorNameOrLabel());
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

void UInventoryComponent::EquipNext()
{
    if (GetOwner()->HasAuthority())
    {
        for (const auto& Item : InventoryList.GetItems())
        {
            if (Item.ItemInstance->GetItemStaticData()->bCanBeEquipped && Item.ItemInstance != CurrentItem)
            {
                if (CurrentItem)
                {
                    UnequipItem();
                }

                EquipItemInstance(Item.ItemInstance);
                return;
            }
            // No item other than currently equipped item can be equipped
            // so this action is a noop
        }
    }
    else
    {
        UE_LOGFMT(LogTemp, Warning, "UInventoryComponent::EquipNext() invoked without Authority");
    }
}

void UInventoryComponent::GameplayEventCallback(const FGameplayEventData* Payload)
{
    // ReSharper disable once CppTooWideScopeInitStatement
    const ENetRole NetRole = GetOwnerRole();
    if (ROLE_Authority == NetRole)
    {
        HandleGameplayEventInternal(*Payload);
    }
    else if (ROLE_AutonomousProxy == NetRole)
    {
        ServerHandleGameplayEvent(*Payload);
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UInventoryComponent::GameplayEventCallback() invoked with Bad NetRole {NetRole}",
                  StaticEnum<ENetRole>()->GetDisplayNameTextByValue(NetRole).ToString());
    }
}

UInventoryItemInstance* UInventoryComponent::GetCurrentItem() const
{
    return CurrentItem.Get();
}
