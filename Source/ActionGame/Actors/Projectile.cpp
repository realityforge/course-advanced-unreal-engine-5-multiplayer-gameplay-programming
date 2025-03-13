

#include "Actors/Projectile.h"
#include "ActionGameStatics.h"
#include "ActionGameTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"

static TAutoConsoleVariable CVarShowProjectiles(TEXT("ShowDebugProjectiles"),
                                                0,
                                                TEXT("Draw debug projectile paths.\n"
                                                     "  0: off\n"
                                                     "  1: on\n"),
                                                ECVF_Cheat);
AProjectile::AProjectile()
{
    bReplicates = true;
    SetReplicatingMovement(true);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->Velocity = FVector::ZeroVector;
    ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

    static const FName Projectile_ProfileName = FName("Projectile");

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
    ProjectileMesh->SetupAttachment(GetRootComponent());
    ProjectileMesh->SetIsReplicated(true);
    ProjectileMesh->SetCollisionProfileName(Projectile_ProfileName);
    ProjectileMesh->bReceivesDecals = false;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (const auto Data = GetProjectileStaticData())
    {
        if (Data->ProjectileMesh)
        {
            ProjectileMesh->SetStaticMesh(Data->ProjectileMesh);
        }
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
            ProjectileMovementComponent->bRotationFollowsVelocity = true;
            ProjectileMovementComponent->InitialSpeed = Data->InitialSpeed;
            ProjectileMovementComponent->MaxSpeed = Data->MaxSpeed;
            ProjectileMovementComponent->Velocity = Data->InitialSpeed * GetActorForwardVector();
            ProjectileMovementComponent->bShouldBounce = false;
            ProjectileMovementComponent->Bounciness = 0.0f;
            ProjectileMovementComponent->ProjectileGravityScale =
                0.0f == Data->GravityMultiplier ? .0001f : Data->GravityMultiplier;
        }
    }

    if (CVarShowProjectiles.GetValueOnGameThread())
    {
        DebugDrawPath();
    }
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (const auto Data = GetProjectileStaticData())
    {
        UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSound, GetActorLocation());
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Data->ImpactEffect, GetActorLocation());
    }

    Super::EndPlay(EndPlayReason);
}

void AProjectile::DebugDrawPath() const
{
    if (const auto Data = GetProjectileStaticData())
    {
        FPredictProjectilePathParams PathParams;
        PathParams.StartLocation = GetActorLocation();
        PathParams.LaunchVelocity = Data->InitialSpeed * GetActorForwardVector();
        PathParams.OverrideGravityZ = Data->GravityMultiplier;
        PathParams.TraceChannel = ECC_Visibility;
        PathParams.bTraceComplex = true;
        PathParams.bTraceWithCollision = true;
        PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
        PathParams.DrawDebugTime = 3.0f;

        // ReSharper disable once CppTooWideScopeInitStatement
        FPredictProjectilePathResult PathResult;
        if (UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult))
        {
            DrawDebugSphere(GetWorld(), PathResult.HitResult.Location, 50.0f, 16, FColor::Red);
        }
    }
}

void AProjectile::OnProjectileStop([[maybe_unused]] const FHitResult& HitResult)
{
    if (const auto Data = GetProjectileStaticData())
    {
        UActionGameStatics::ApplyRadialDamage(this,
                                              GetOwner(),
                                              GetActorLocation(),
                                              Data->DamageRadius,
                                              Data->BaseDamage,
                                              Data->DamageEffects,
                                              Data->RadialDamageQueryTypes,
                                              Data->RadialDamageTraceType);
    }

    Destroy();
}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
    return IsValid(ProjectileDataClass) ? GetDefault<UProjectileStaticData>(ProjectileDataClass) : nullptr;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectile, ProjectileDataClass);
}
