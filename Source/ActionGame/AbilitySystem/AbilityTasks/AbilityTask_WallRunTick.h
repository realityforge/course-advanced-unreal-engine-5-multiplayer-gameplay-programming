#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "CoreMinimal.h"
#include "AbilityTask_WallRunTick.generated.h"

class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunWallSideDeterminedDelegate, bool, bLeftSide);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate);

UCLASS()
class ACTIONGAME_API UAbilityTask_WallRunTick : public UAbilityTask
{
    friend class UGameplayAbility_WallRun;

    GENERATED_BODY()

    UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = true))
    FOnWallRunFinishedDelegate OnWallRunFinished;

    UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = true))
    FOnWallRunWallSideDeterminedDelegate OnWallRunWallSideDetermined;

    UPROPERTY()
    TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent{ nullptr };
    UPROPERTY()
    TObjectPtr<ACharacter> Character{ nullptr };
    UPROPERTY()
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

    bool FindRunnableWall(FHitResult& Hit) const;

    bool IsWallOnTheLeftSide(const FHitResult& InWallHit) const;

public:
    UFUNCTION(BlueprintCallable,
              Category = "Ability|Tasks",
              meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
    static UAbilityTask_WallRunTick* CreateWallRunTask(
        UGameplayAbility* OwningAbility,
        UPARAM(DisplayName = "Character") ACharacter* InCharacter,
        UPARAM(DisplayName = "CharacterMovementComponent") UCharacterMovementComponent* InCharacterMovementComponent,
        UPARAM(DisplayName = "TraceObjectTypes") const TArray<TEnumAsByte<EObjectTypeQuery>>& InTraceObjectTypes);

    virtual void Activate() override;

    virtual void OnDestroy(bool bInOwnerFinished) override;

    virtual void TickTask(float DeltaTime) override;

    FORCEINLINE const FOnWallRunFinishedDelegate& GetOnWallRunFinished() const { return OnWallRunFinished; }

    // UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = true))
    // FOnWallRunWallSideDeterminedDelegate OnWallRunWallSideDetermined;
};
