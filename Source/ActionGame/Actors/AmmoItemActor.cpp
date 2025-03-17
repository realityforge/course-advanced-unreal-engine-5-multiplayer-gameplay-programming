#include "Actors/AmmoItemActor.h"
#include "Inventory/InventoryItemInstance.h"

void AAmmoItemActor::InitInternal()
{
    if (const auto Data = GetAmmoItemStaticData())
    {
        if (Data->StaticMesh)
        {
            if (auto StaticMesh = NewObject<UStaticMeshComponent>(this, "MeshComponent"))
            {
                StaticMesh->RegisterComponent();
                StaticMesh->SetStaticMesh(Data->StaticMesh);
                StaticMesh->AttachToComponent(GetRootComponent(),
                                              FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeshComponent = StaticMesh;
            }
        }
    }
}

const UAmmoItemStaticData* AAmmoItemActor::GetAmmoItemStaticData() const
{
    return GetItemInstance() ? Cast<UAmmoItemStaticData>(GetItemInstance()->GetItemStaticData()) : nullptr;
}

AAmmoItemActor::AAmmoItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
}
