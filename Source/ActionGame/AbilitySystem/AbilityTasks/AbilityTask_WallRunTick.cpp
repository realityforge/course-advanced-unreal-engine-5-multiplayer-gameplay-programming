#include "AbilitySystem/AbilityTasks/AbilityTask_WallRunTick.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

bool UAbilityTask_WallRunTick::FindRunnableWall(FHitResult& Hit) const
{
    const FVector CharacterLocation = Character->GetActorLocation();
    const FVector RightVector = Character->GetActorRightVector();
    const FVector ForwardVector = Character->GetActorForwardVector();
    // slightly bigger than the capsule
    const float TraceLength = Character->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.0f;

    const TArray<AActor*> IgnoreActors{ Character };

    static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
    check(CVar);
    const bool bShowDebugTraversal = CVar->GetBool();

    const auto DrawDebugType = bShowDebugTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    const auto World = GetWorld();

    // If there is an obstacle immediately in front of the character, then end the WallRun
    if (UKismetSystemLibrary::LineTraceSingleForObjects(World,
                                                        CharacterLocation,
                                                        CharacterLocation + ForwardVector * TraceLength,
                                                        TraceObjectTypes,
                                                        true,
                                                        IgnoreActors,
                                                        DrawDebugType,
                                                        Hit,
                                                        false))
    {
        return false;
    }

    // Is there a wall to the left of the character?
    if (UKismetSystemLibrary::LineTraceSingleForObjects(World,
                                                        CharacterLocation,
                                                        CharacterLocation + -RightVector * TraceLength,
                                                        TraceObjectTypes,
                                                        true,
                                                        IgnoreActors,
                                                        DrawDebugType,
                                                        Hit,
                                                        false))
    {
        if (FVector::DotProduct(Hit.ImpactNormal, RightVector) > 0.3f)
        {
            return true;
        }
    }

    // Is there a wall to the right of the character?
    if (UKismetSystemLibrary::LineTraceSingleForObjects(World,
                                                        CharacterLocation,
                                                        CharacterLocation + RightVector * TraceLength,
                                                        TraceObjectTypes,
                                                        true,
                                                        IgnoreActors,
                                                        DrawDebugType,
                                                        Hit,
                                                        false))
    {
        if (FVector::DotProduct(Hit.ImpactNormal, -RightVector) > 0.3f)
        {
            return true;
        }
    }

    return false;
}

bool UAbilityTask_WallRunTick::IsWallOnTheLeftSide(const FHitResult& InWallHit) const
{
    return FVector::DotProduct(Character->GetActorRightVector(), InWallHit.ImpactNormal) > 0.0f;
}

UAbilityTask_WallRunTick*
UAbilityTask_WallRunTick::CreateWallRunTask(UGameplayAbility* OwningAbility,
                                            ACharacter* InCharacter,
                                            UCharacterMovementComponent* InCharacterMovementComponent,
                                            const TArray<TEnumAsByte<EObjectTypeQuery>>& InTraceObjectTypes)
{
    const auto Task = NewAbilityTask<UAbilityTask_WallRunTick>(OwningAbility);
    Task->Character = InCharacter;
    Task->CharacterMovementComponent = InCharacterMovementComponent;
    Task->TraceObjectTypes = InTraceObjectTypes;
    Task->bTickingTask = true;
    return Task;
}

void UAbilityTask_WallRunTick::Activate()
{
    if (FHitResult Hit; FindRunnableWall(Hit))
    {
        // Found a wall to run on, so let's run on it!

        // notify delegates if appropriate
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnWallRunWallSideDetermined.Broadcast(IsWallOnTheLeftSide(Hit));
        }

        // Calling landed so we can be registered as "landing" on the wall.
        // This also helps end all InAir effects added in ActionGameCharacter
        Character->Landed(Hit);

        // Move the character so that they are just outside the wall.
        // The number is just a magic number to try and stop the character intersecting
        // the wall and getting stuck
        Character->SetActorLocation(Hit.ImpactPoint + Hit.ImpactNormal * 60.0f);

        // Flying means that we no longer have gravity or friction applied
        CharacterMovementComponent->SetMovementMode(MOVE_Flying);
    }
    else
    {
        // Failed to find wall to hit so wall run will fail
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnWallRunFinished.Broadcast();
        }

        EndTask();
    }
}

void UAbilityTask_WallRunTick::OnDestroy(const bool bInOwnerFinished)
{
    CharacterMovementComponent->SetPlaneConstraintEnabled(false);
    CharacterMovementComponent->SetMovementMode(MOVE_Falling);

    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WallRunTick::TickTask(const float DeltaTime)
{
    if (FHitResult Hit; FindRunnableWall(Hit))
    {
        // Still a wall underfoot
        const FRotator DirectionRotator = IsWallOnTheLeftSide(Hit) ? FRotator(0, -90, 0) : FRotator(0, 90, 0);
        const FVector WallRunDirection = DirectionRotator.RotateVector(Hit.ImpactNormal);
        Character->SetActorRotation(WallRunDirection.Rotation());

        // 700 is just some arbitrary speed we are running in
        CharacterMovementComponent->Velocity = WallRunDirection * 700.0f;

        // z set to 0 means we do not drop while wall running, otherwise it can be some factor of gravity
        constexpr bool bApplyGravity = false;
        const auto Gravity = CharacterMovementComponent->GetGravityZ() * DeltaTime;
        CharacterMovementComponent->Velocity.Z = bApplyGravity ? Gravity : 0;
        CharacterMovementComponent->SetPlaneConstraintEnabled(true);
        CharacterMovementComponent->SetPlaneConstraintOrigin(Hit.ImpactPoint);
        CharacterMovementComponent->SetPlaneConstraintNormal(Hit.ImpactNormal);
    }
    else
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnWallRunFinished.Broadcast();
        }

        EndTask();
    }
}
