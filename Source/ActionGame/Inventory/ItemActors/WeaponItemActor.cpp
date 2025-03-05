#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

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

FVector AWeaponItemActor::GetMuzzleLocation() const
{
    static FName MuzzleSocketName("Muzzle");
    return MeshComponent ? MeshComponent->GetSocketLocation(MuzzleSocketName) : FVector::ZeroVector;
}

void AWeaponItemActor::PlayWeaponEffects(const FHitResult& HitResult)
{
    if (HasAuthority())
    {
        MulticastPlayWeaponEffects(HitResult);
    }
    else
    {
        PlayWeaponEffectsInternal(HitResult);
    }
}

void AWeaponItemActor::MulticastPlayWeaponEffects_Implementation(const FHitResult& HitResult)
{
    if (!Owner || ROLE_AutonomousProxy != Owner->GetLocalRole())
    {
        // We make sure we do not play the effects twice by checking
        // it is not an AutonomousProxy (i.e. the one who called PlayWeaponEffects
        // that also played locally ... wait does this make sense?)
        PlayWeaponEffectsInternal(HitResult);
    }
}

void AWeaponItemActor::PlayWeaponEffectsInternal(const FHitResult& HitResult) const
{
    if (HitResult.PhysMaterial.Get())
    {
        if (const auto Material = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get()))
        {
            UGameplayStatics::PlaySoundAtLocation(this, Material->GetPointImpactSound(), HitResult.ImpactPoint);
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
                                                           Material->GetPointImpactEffect(),
                                                           HitResult.ImpactPoint);
        }
    }
    if (const auto Data = GetWeaponStaticData())
    {
        UGameplayStatics::PlaySoundAtLocation(this, Data->AttackSound, GetActorLocation());
    }
}
