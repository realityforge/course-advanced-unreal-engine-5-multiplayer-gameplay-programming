#pragma once

#include "Actors/ItemActor.h"
#include "CoreMinimal.h"
#include "WeaponItemActor.generated.h"

UCLASS()
class ACTIONGAME_API AWeaponItemActor : public AItemActor
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UMeshComponent> MeshComponent{ nullptr };

    const UWeaponStaticData* GetWeaponStaticData() const;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayWeaponEffects(const FHitResult& HitResult);

    void PlayWeaponEffectsInternal(const FHitResult& HitResult) const;

protected:
    virtual void InitInternal() override;

public:
    AWeaponItemActor();

    UFUNCTION(BlueprintPure, BlueprintCallable)
    FVector GetMuzzleLocation() const;

    UFUNCTION(BlueprintCallable)
    void PlayWeaponEffects(const FHitResult& HitResult);
};
