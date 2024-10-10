#include "ActorComponents/FootstepsComponent.h"
#include "ActionGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

static TAutoConsoleVariable CVarShowDebugFootsteps(TEXT("ShowDebugFootsteps") /* name */,
                                                   0 /* Default Value */,
                                                   TEXT("Draws debug info about footsteps:\n") TEXT("  0: off\n")
                                                       TEXT("  1: on\n"),
                                                   ECVF_Cheat);

UFootstepsComponent::UFootstepsComponent() {}

void UFootstepsComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UFootstepsComponent::HandleFootStep(const EFoot Foot)
{
    if (const auto Character = Cast<AActionGameCharacter>(GetOwner()))
    {
        if (const auto Mesh = Character->GetMesh())
        {
            const FName SocketName = EFoot::Left == Foot ? LeftFootSocketName : RightFootSocketName;
            const FVector SocketLocation = Mesh->GetSocketLocation(SocketName);
            const FVector TraceStart = SocketLocation + FVector::UpVector * 20.f;
            const FVector TraceEnd = TraceStart + FVector::UpVector * -50.f;

            FCollisionQueryParams Params;
            Params.bReturnPhysicalMaterial = true;
            Params.AddIgnoredActor(Character);

            const bool bShowDebugFootsteps = CVarShowDebugFootsteps.GetValueOnAnyThread() > 0;

            // ReSharper disable once CppTooWideScopeInitStatement
            FHitResult HitResult;
            if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, Params))
            {
                if (HitResult.bBlockingHit)
                {
                    if (const auto Material = HitResult.PhysMaterial.Get())
                    {
                        if (const auto PhysicalMaterial = Cast<UAG_PhysicalMaterial>(Material))
                        {
                            if (const auto Sound = PhysicalMaterial->GetFootstepSound())
                            {
                                UGameplayStatics::PlaySoundAtLocation(this, Sound, HitResult.Location);
                            }
                        }
                        if (bShowDebugFootsteps)
                        {
                            // Draw name of material at hit location
                            DrawDebugString(GetWorld(),
                                            HitResult.Location,
                                            *Material->GetName(),
                                            nullptr,
                                            FColor::White,
                                            4.f);
                        }
                    }
                    if (bShowDebugFootsteps)
                    {
                        // Draw a sphere at Hit Location
                        DrawDebugSphere(GetWorld(), HitResult.Location, 16, 16, FColor::Red, false, 4.f);
                    }
                }
                else
                {
                    if (bShowDebugFootsteps)
                    {
                        // Draw a line representing trace  if there is no hit
                        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 4.f, 0, 1);
                    }
                }
            }
        }
    }
}
