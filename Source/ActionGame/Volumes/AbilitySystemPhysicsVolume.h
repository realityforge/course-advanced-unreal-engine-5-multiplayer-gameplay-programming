#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "AbilitySystemPhysicsVolume.generated.h"

class UGameplayAbility;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UGameplayEffect;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
    GENERATED_BODY()

    TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

    TArray<FActiveGameplayEffectHandle> AppliedEffects;
};

UCLASS()
class ACTIONGAME_API AAbilitySystemPhysicsVolume : public APhysicsVolume
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGameplayTag> GameplayEventsToSendOnEnter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGameplayTag> GameplayEventsToSendOnExit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDrawDebug{ false };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayAbility>> OngoingAbilitiesToGive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

    UPROPERTY(Transient)
    TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;

public:
    AAbilitySystemPhysicsVolume();

    virtual void Tick(float DeltaSeconds) override;

    virtual void ActorEnteredVolume(AActor* Other) override;

    virtual void ActorLeavingVolume(AActor* Other) override;
};
