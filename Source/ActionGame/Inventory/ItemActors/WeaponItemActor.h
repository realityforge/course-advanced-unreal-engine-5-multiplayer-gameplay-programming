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

protected:
    virtual void InitInternal();

public:
    AWeaponItemActor();

    UFUNCTION(BlueprintPure, BlueprintCallable)
    FVector GetMuzzleLocation() const;
};
