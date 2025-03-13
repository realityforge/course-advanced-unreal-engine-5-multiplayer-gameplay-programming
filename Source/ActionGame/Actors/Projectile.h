

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileStaticData;

UCLASS()
class ACTIONGAME_API AProjectile : public AActor
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY()
    class UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> ProjectileMesh;

    void DebugDrawPath() const;

    UFUNCTION()
    void OnProjectileStop(const FHitResult& HitResult);

public:
    AProjectile();

    const UProjectileStaticData* GetProjectileStaticData() const;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TSubclassOf<UProjectileStaticData> ProjectileDataClass;
};
