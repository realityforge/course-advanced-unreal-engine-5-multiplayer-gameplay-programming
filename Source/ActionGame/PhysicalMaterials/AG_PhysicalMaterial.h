#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AG_PhysicalMaterial.generated.h"

UCLASS()
class ACTIONGAME_API UAG_PhysicalMaterial : public UPhysicalMaterial
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Material", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> FootstepSound{ nullptr };

public:
    FORCEINLINE USoundBase* GetFootstepSound() const { return FootstepSound; }
};
