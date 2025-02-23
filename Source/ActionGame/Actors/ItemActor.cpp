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
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
}

void AItemActor::Init(UInventoryItemInstance* InItemInstance)
{
    ItemInstance = InItemInstance;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AItemActor::OnRep_ItemState()
{
    if (EItemState::Equipped == ItemState)
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
            ItemInstance->Init(ItemStaticDataClass);

            SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            SphereComponent->SetGenerateOverlapEvents(true);
        }
    }
}

void AItemActor::OnEquipped()
{
    ItemState = EItemState::Equipped;

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnUnequipped()
{
    ItemState = EItemState::None;

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnDropped()
{
    UE_LOG(LogTemp, Display, TEXT("OnDropped"));
    ItemState = EItemState::Dropped;
    GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    // Trace from current position to ground and push the item in front
    // of the character to get the drop location
    if (const auto ItemOwner = GetOwner())
    {
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
        static const FName ProfileName("WorldStatic");
        UKismetSystemLibrary::LineTraceSingleByProfile(Owner,
                                                       TraceStart,
                                                       TraceEnd,
                                                       ProfileName,
                                                       true,
                                                       IgnoreActors,
                                                       DrawDebugType,
                                                       HitResult,
                                                       true);

        SetActorLocation(HitResult.bBlockingHit ? HitResult.Location : TraceEnd);
    }

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetGenerateOverlapEvents(true);
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemActor, ItemInstance);
    DOREPLIFETIME(AItemActor, ItemState);
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
