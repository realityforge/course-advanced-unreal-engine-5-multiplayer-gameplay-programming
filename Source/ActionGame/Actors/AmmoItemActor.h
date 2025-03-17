#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "AmmoItemActor.generated.h"

class UAmmoItemStaticData;

UCLASS()
class ACTIONGAME_API AAmmoItemActor : public AItemActor
{
    GENERATED_BODY()

public:
    AAmmoItemActor();

protected:
    UPROPERTY()
    TObjectPtr<UMeshComponent> MeshComponent{ nullptr };

    virtual void InitInternal() override;

public:
    const UAmmoItemStaticData* GetAmmoItemStaticData() const;
};
