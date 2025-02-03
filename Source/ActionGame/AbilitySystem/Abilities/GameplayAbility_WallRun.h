#pragma once

#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "CoreMinimal.h"
#include "GameplayAbility_WallRun.generated.h"

class UAbilityTask_WallRunTick;

/**
 * Gameplay ability to perform Wall Running.
 */
UCLASS(Abstract)
class ACTIONGAME_API UGameplayAbility_WallRun : public UAG_GameplayAbility
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> LeftSideEffectClass{ nullptr };

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> RightSideEffectClass{ nullptr };

    UPROPERTY(EditDefaultsOnly)
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes{};

    UPROPERTY()
    TObjectPtr<UAbilityTask_WallRunTick> WallRunTask{ nullptr };

    UFUNCTION()
    void OnCapsuleComponentHit(UPrimitiveComponent* HitComponent,
                               AActor* OtherActor,
                               UPrimitiveComponent* OtherComp,
                               FVector NormalImpulse,
                               const FHitResult& Hit);

    UFUNCTION()
    void OnWallSideDetermined(bool bLeftSide);

public:
    UGameplayAbility_WallRun();

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;
};
