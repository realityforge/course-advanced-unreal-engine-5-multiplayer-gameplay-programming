#include "AbilitySystem/Abilities/GameplayAbility_WallRun.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_WallRunTick.h"
#include "AbilitySystemComponent.h"
#include "ActionGameCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Logging/StructuredLog.h"

UGameplayAbility_WallRun::UGameplayAbility_WallRun()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayAbility_WallRun::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent,
                                                     AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp,
                                                     FVector NormalImpulse,
                                                     const FHitResult& Hit)
{
    // If we overlap a candidate actor, then try to start WallRunning.
    if (auto* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
    {
        AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpec()->Handle);
    }
}

void UGameplayAbility_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    // This differs from normal abilities as we add a OnComponentHit handler when we
    // grant the ability and then if we have an overlap we try to activate the ability

    const AActor* AvatarActor = ActorInfo->AvatarActor.Get();
    if (const auto* Character = Cast<ACharacter>(AvatarActor))
    {
        if (auto* CapsuleComponent = Character->GetCapsuleComponent())
        {
            CapsuleComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnCapsuleComponentHit);
        }
        else
        {
            UE_LOGFMT(LogTemp,
                      Error,
                      "UGameplayAbility_WallRun::OnGiveAbility unable to retrieve CapsuleComponent "
                      "from AvatarActor '{Name}'",
                      GetNameSafe(AvatarActor));
        }
    }
    else
    {
        UE_LOGFMT(LogTemp,
                  Error,
                  "UGameplayAbility_WallRun::OnGiveAbility invoked when AvatarActor '{Name}' "
                  "is not a Character instance of the type '{ActorType}'",
                  GetNameSafe(AvatarActor),
                  AvatarActor ? AvatarActor->GetClass()->GetName() : TEXT(""));
    }
}

void UGameplayAbility_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilitySpec& Spec)
{
    if (ActorInfo)
    {
        if (const auto* Character = Cast<ACharacter>(ActorInfo->AvatarActor))
        {
            if (auto* CapsuleComponent = Character->GetCapsuleComponent())
            {
                // Remove callback added in OnGiveAbility
                CapsuleComponent->OnComponentHit.RemoveDynamic(this, &ThisClass::OnCapsuleComponentHit);
            }
        }
    }
    Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGameplayAbility_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayTagContainer* SourceTags,
                                                  const FGameplayTagContainer* TargetTags,
                                                  FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        const auto* Character = GetActionGameCharacterFromActorInfo();
        return Character && !Character->GetCharacterMovement()->IsMovingOnGround();
    }
    else
    {
        return false;
    }
}

void UGameplayAbility_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    auto* Character = GetActionGameCharacterFromActorInfo();
    auto* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;

    WallRunTask = UAbilityTask_WallRunTick::CreateWallRunTask(this, Character, CharacterMovement, TraceObjectTypes);
    WallRunTask->OnWallRunFinished.AddDynamic(this, &ThisClass::K2_EndAbility);
    WallRunTask->OnWallRunWallSideDetermined.AddDynamic(this, &ThisClass::OnWallSideDetermined);
    WallRunTask->ReadyForActivation();
}

void UGameplayAbility_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const bool bReplicateEndAbility,
                                          const bool bWasCancelled)
{
    if (IsValid(WallRunTask))
    {
        WallRunTask->EndTask();
    }
    if (auto* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
    {
        AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(LeftSideEffectClass, AbilitySystemComponent);
        AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(RightSideEffectClass, AbilitySystemComponent);
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayAbility_WallRun::OnWallSideDetermined(const bool bLeftSide)
{
    if (const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
    {
        const FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
        const auto EffectClass = bLeftSide ? LeftSideEffectClass : RightSideEffectClass;
        AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EffectClass, 1, EffectContext);
    }
}
