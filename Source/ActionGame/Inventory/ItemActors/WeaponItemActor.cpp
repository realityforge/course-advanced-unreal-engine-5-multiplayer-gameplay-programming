#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Inventory/InventoryItemInstance.h"
#include "Logging/StructuredLog.h"

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
    const auto Instance = GetItemInstance();
    return Instance ? Cast<UWeaponStaticData>(Instance->GetItemStaticData()) : nullptr;
}

void AWeaponItemActor::InitInternal()
{
    if (const auto Data = GetWeaponStaticData())
    {
        if (Data->SkeletalMesh)
        {
            auto SkeletalMeshComponent =
                NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), "MeshComponent");
            if (SkeletalMeshComponent)
            {
                SkeletalMeshComponent->RegisterComponent();
                SkeletalMeshComponent->SetSkeletalMesh(Data->SkeletalMesh);
                SkeletalMeshComponent->AttachToComponent(GetRootComponent(),
                                                         FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeshComponent = SkeletalMeshComponent;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AWeaponItemActor::InitInternal(): Failed to create skeletal component"));
            }
        }
        else if (Data->StaticMesh)
        {
            auto StaticMeshComponent =
                NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), "MeshComponent");
            if (StaticMeshComponent)
            {
                StaticMeshComponent->RegisterComponent();
                StaticMeshComponent->SetStaticMesh(Data->StaticMesh);
                StaticMeshComponent->AttachToComponent(GetRootComponent(),
                                                       FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeshComponent = StaticMeshComponent;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AWeaponItemActor::InitInternal(): Failed to create skeletal component"));
            }
        }
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Warning,
                  "AWeaponItemActor::InitInternal(): ItemInstance is invalid or does "
                  "not contain a reference to WeaponStaticData");
    }
}

AWeaponItemActor::AWeaponItemActor() {}
