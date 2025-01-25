#pragma once

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "CoreMinimal.h"
#include "GameplayAbility_Vault.generated.h"

UCLASS(Abstract)
class ACTIONGAME_API UGameplayAbility_Vault : public UAG_GameplayAbility
{
    static const FName NAME_JumpToLocation;
    static const FName NAME_JumpOverLocation;

    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float HorizontalTraceRadius{ 30.f };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float HorizontalTraceLength{ 500.f };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float HorizontalTraceCount{ 5.f };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float HorizontalTraceStep{ 60.f };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float VerticalTraceRadius{ 30.f };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float VerticalTraceStep{ 30.f };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

    UPROPERTY(EditDefaultsOnly, Category = "Trace", meta = (RuleRangerRequired))
    TObjectPtr<UAnimMontage> VaultMontage{ nullptr };

    FVector JumpToLocation;
    FVector JumpOverLocation;

    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask{ nullptr };

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;

public:
    UGameplayAbility_Vault();

    virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle,
                             const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo,
                             FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
};
