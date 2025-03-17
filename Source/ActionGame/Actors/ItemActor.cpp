#include "Actors/ItemActor.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameGamePlayTags.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.bAllowTickOnDedicatedServer = false;
    PrimaryActorTick.bTickEvenWhenPaused = false;
    bReplicates = true;

    // Set replicate movement to true so can add physics to item when dropping it etc
    SetReplicatingMovement(true);

    SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
    SetRootComponent(SphereComponent);
}

void AItemActor::Init(UInventoryItemInstance* InItemInstance)
{
    ItemInstance = InItemInstance;
    InitInternal();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AItemActor::OnRep_ItemState()
{
    UpdateSphereCollision();
}

void AItemActor::OnRep_ItemInstance(const UInventoryItemInstance* OldItemInstance)
{
    if (IsValid(ItemInstance) && !IsValid(OldItemInstance))
    {
        // If we get here - this is the first time this actor has replicated
        // as Instance does not change
        InitInternal();
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AItemActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                         AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex,
                                         bool bFromSweep,
                                         const FHitResult& SweepResult)
{
    FGameplayEventData Payload;
    Payload.Instigator = this;
    Payload.OptionalObject = ItemInstance;
    Payload.EventTag = ActionGameGameplayTags::Event_Inventory_EquipItemActor;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, Payload.EventTag, Payload);
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        if (!IsValid(ItemInstance) && IsValid(ItemStaticDataClass))
        {
            ItemInstance = NewObject<UInventoryItemInstance>();
            ItemInstance->Init(ItemStaticDataClass, Quantity);

            UpdateSphereCollision();

            InitInternal();
        }
    }
}

void AItemActor::InitInternal()
{
    // Do nothing here but subclasses may use it to initialize
}

void AItemActor::UpdateSphereCollision() const
{
    if (EItemState::Equipped == ItemState || EItemState::None == ItemState)
    {
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        SphereComponent->SetGenerateOverlapEvents(false);
    }
    else
    {
        check(EItemState::Dropped == ItemState);
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        SphereComponent->SetGenerateOverlapEvents(true);
    }
}

void AItemActor::OnEquipped()
{
    ItemState = EItemState::Equipped;
    UpdateSphereCollision();
}

void AItemActor::OnUnequipped()
{
    ItemState = EItemState::None;
    UpdateSphereCollision();
}

void AItemActor::OnDropped()
{
    // UE_LOGFMT(LogTemp, Display, "{Item} OnDropped Owner={Owner}", GetActorNameOrLabel(), GetNameSafe(GetOwner()));

    // Trace from current position to ground and push the item in front
    // of the character to get the drop location
    if (const auto ItemOwner = GetOwner())
    {
        ItemState = EItemState::Dropped;
        GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

        constexpr float ForwardDistance = 100.0f;
        constexpr float MaxDistanceToGround = 1000.0f;

        const FVector Location = GetActorLocation();
        const FVector Forward = ItemOwner->GetActorForwardVector();
        const FVector TraceStart = Location + Forward * ForwardDistance;
        const FVector TraceEnd = TraceStart * -FVector::UpVector * MaxDistanceToGround;
        static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugInventory"));
        const EDrawDebugTrace::Type DrawDebugType =
            CVar->GetBool() ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

        const TArray IgnoreActors{ ItemOwner };
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingleByProfile(Owner,
                                                       TraceStart,
                                                       TraceEnd,
                                                       UCollisionProfile::BlockAll_ProfileName,
                                                       true,
                                                       IgnoreActors,
                                                       DrawDebugType,
                                                       HitResult,
                                                       true);

        SetActorLocation(HitResult.bBlockingHit ? HitResult.Location : TraceEnd);

        UpdateSphereCollision();
    }
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemActor, ItemInstance);
    DOREPLIFETIME(AItemActor, ItemState);
    DOREPLIFETIME(AItemActor, Quantity);
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bReplicatedSubobjects = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
    if (IsValid(ItemInstance))
    {
        bReplicatedSubobjects |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
    }
    return bReplicatedSubobjects;
}
