#include "AbilitySystem/Abilities/GameplayAbility_Vault.h"
#include "ActionGameCharacter.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

const FName UGameplayAbility_Vault::NAME_JumpToLocation("JumpToLocation");
const FName UGameplayAbility_Vault::NAME_JumpOverLocation("JumpOverLocation");

UGameplayAbility_Vault::UGameplayAbility_Vault()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGameplayAbility_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         FGameplayTagContainer* OptionalRelevantTags)
{
    if (Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
    {
        if (auto Character = GetActionGameCharacterFromActorInfo())
        {
            FVector StartLocation = Character->GetActorLocation();
            FVector ForwardVector = Character->GetActorForwardVector();
            FVector UpVector = Character->GetActorUpVector();

            TArray<AActor*> IgnoreActors{ Character };

            static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
            check(CVar);
            const bool bShowDebugTraversal = CVar->GetBool();

            const EDrawDebugTrace::Type DebugDrawType =
                bShowDebugTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

            bool bJumpToLocationSet = false;
            int32 JumpToLocationIndex = INDEX_NONE;

            int32 Index = 0;

            FHitResult Hit;

            float MaxJumpDistance{ HorizontalTraceLength };
            for (; Index < HorizontalTraceCount; ++Index)
            {
                const FVector TraceStart = StartLocation + Index * UpVector * HorizontalTraceStep;
                // ReSharper disable once CppTooWideScopeInitStatement
                const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;
                if (UKismetSystemLibrary::SphereTraceSingleForObjects(this,
                                                                      TraceStart,
                                                                      TraceEnd,
                                                                      HorizontalTraceRadius,
                                                                      TraceObjectTypes,
                                                                      true,
                                                                      IgnoreActors,
                                                                      DebugDrawType,
                                                                      Hit,
                                                                      true))
                {
                    if (INDEX_NONE == JumpToLocationIndex && Index < HorizontalTraceCount - 1)
                    {
                        JumpToLocationIndex = Index;
                        JumpToLocation = Hit.Location;
                    }
                    else if (JumpToLocationIndex == Index - 1)
                    {
                        MaxJumpDistance = FVector::Dist(TraceStart, Hit.Location);
                        break;
                    }
                }
                else if (INDEX_NONE != JumpToLocationIndex)
                {
                    break;
                }
            }
            if (INDEX_NONE == JumpToLocationIndex)
            {
                // Found nothing
                return false;
            }

            const float DistanceToJumpTo = FVector::Dist(StartLocation, JumpToLocation);
            const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJumpTo;
            if (MaxVerticalTraceDistance < 0)
            {
                return false;
            }

            if (Index == HorizontalTraceCount)
            {
                Index = HorizontalTraceCount - 1;
            }

            const float VerticalTraceLength =
                FMath::Abs(JumpToLocation.Z - (StartLocation + Index * UpVector * HorizontalTraceStep).Z);

            FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;

            const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;

            Index = 0;

            bool bJumpOverLocationSet = false;
            for (; Index < VerticalTraceCount; ++Index)
            {
                const FVector TraceStart = StartLocation + Index * ForwardVector * VerticalTraceStep;
                const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength;
                if (UKismetSystemLibrary::SphereTraceSingleForObjects(this,
                                                                      TraceStart,
                                                                      TraceEnd,
                                                                      VerticalTraceRadius,
                                                                      TraceObjectTypes,
                                                                      true,
                                                                      IgnoreActors,
                                                                      DebugDrawType,
                                                                      Hit,
                                                                      true))
                {
                    JumpOverLocation = Hit.ImpactPoint;
                    if (0 == Index)
                    {
                        JumpToLocation = JumpOverLocation;
                    }
                }
                else if (0 != Index)
                {
                    bJumpOverLocationSet = true;
                    break;
                }
            }
            if (!bJumpOverLocationSet)
            {
                return false;
            }

            const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;
            if (UKismetSystemLibrary::SphereTraceSingleForObjects(this,
                                                                  TraceStart,
                                                                  JumpOverLocation,
                                                                  VerticalTraceRadius,
                                                                  TraceObjectTypes,
                                                                  true,
                                                                  IgnoreActors,
                                                                  DebugDrawType,
                                                                  Hit,
                                                                  true))
            {
                JumpOverLocation = Hit.ImpactPoint;
            }

            if (bShowDebugTraversal)
            {
                DrawDebugSphere(GetWorld(), JumpToLocation, 15, 16, FColor::White, false, 7.f);
                DrawDebugSphere(GetWorld(), JumpOverLocation, 15, 16, FColor::White, false, 7.f);
            }
            return true;
        }
    }

    return false;
}

void UGameplayAbility_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (CommitCheck(Handle, ActorInfo, ActivationInfo))
    {
        if (const auto Character = GetActionGameCharacterFromActorInfo())
        {
            if (const auto CharacterMovement = Character->GetCharacterMovement())
            {
                // If we do not set to flying, we can be impacted by gravity and friction which can causes problems
                CharacterMovement->SetMovementMode(MOVE_Flying);
            }
            else
            {
                UE_LOG(LogTemp,
                       Error,
                       TEXT("UGameplayAbility_Vault::ActivateAbility: Character->GetCharacterMovement() is null"));
            }
            if (const auto CapsuleComponent = Character->GetCapsuleComponent())
            {
                // Explicitly ignore channels that have been marked as ignored
                for (auto Channel : CollisionChannelsToIgnore)
                {
                    CapsuleComponent->SetCollisionResponseToChannel(Channel, ECR_Ignore);
                }
            }
            else
            {
                UE_LOG(LogTemp,
                       Error,
                       TEXT("UGameplayAbility_Vault::ActivateAbility: Character->GetCapsuleComponent() is null"));
            }
            if (const auto MotionWarpingComponent = Character->GetMotionWarpingComponent())
            {
                // We explicitly set the rotation ... in case other montages attempt to update the rotation
                MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(NAME_JumpToLocation,
                                                                                     JumpToLocation,
                                                                                     Character->GetActorRotation());
                MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(NAME_JumpOverLocation,
                                                                                     JumpOverLocation,
                                                                                     Character->GetActorRotation());
            }
            else
            {
                UE_LOG(LogTemp,
                       Error,
                       TEXT("UGameplayAbility_Vault::ActivateAbility: Character->GetMotionWarpingComponent() is null"));
            }
            MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, VaultMontage);
            MontageTask->OnBlendOut.AddDynamic(this, &UGameplayAbility_Vault::K2_EndAbility);
            MontageTask->OnInterrupted.AddDynamic(this, &UGameplayAbility_Vault::K2_EndAbility);
            MontageTask->OnCompleted.AddDynamic(this, &UGameplayAbility_Vault::K2_EndAbility);
            MontageTask->OnCancelled.AddDynamic(this, &UGameplayAbility_Vault::K2_EndAbility);
            MontageTask->ReadyForActivation();
        }
        else
        {
            UE_LOG(LogTemp,
                   Error,
                   TEXT("UGameplayAbility_Vault::ActivateAbility: GetActionGameCharacterFromActorInfo() is null"));
        }
    }
    else
    {
        K2_EndAbility();
    }
}

void UGameplayAbility_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility,
                                        bool bWasCancelled)
{
    if (IsValid(MontageTask))
    {
        MontageTask->EndTask();
    }
    if (const auto Character = GetActionGameCharacterFromActorInfo())
    {
        if (const auto MotionWarpingComponent = Character->GetMotionWarpingComponent())
        {
            // Clear targets
            MotionWarpingComponent->RemoveWarpTarget(NAME_JumpToLocation);
            MotionWarpingComponent->RemoveWarpTarget(NAME_JumpOverLocation);
        }
        if (const auto CapsuleComponent = Character->GetCapsuleComponent())
        {
            // Explicitly ignore channels that have been marked as ignored
            for (auto Channel : CollisionChannelsToIgnore)
            {
                CapsuleComponent->SetCollisionResponseToChannel(Channel, ECR_Block);
            }
        }
        if (const auto CharacterMovement = Character->GetCharacterMovement())
        {
            // Additional check in case something happened and we are no longer "flying"
            if (CharacterMovement->IsFlying())
            {
                // Set this to falling so that after vaulting we return to gravity impacting us
                CharacterMovement->SetMovementMode(MOVE_Falling);
            }
        }
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
