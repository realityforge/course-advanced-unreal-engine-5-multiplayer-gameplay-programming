#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AG_PhysicalMaterial.generated.h"

class UNiagaraSystem;

UCLASS()
class ACTIONGAME_API UAG_PhysicalMaterial : public UPhysicalMaterial
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Material", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> FootstepSound{ nullptr };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Material", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> PointImpactSound{ nullptr };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Material", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraSystem> PointImpactEffect{ nullptr };

public:
    FORCEINLINE USoundBase* GetFootstepSound() const { return FootstepSound; }
    FORCEINLINE USoundBase* GetPointImpactSound() const { return PointImpactSound; }
    FORCEINLINE UNiagaraSystem* GetPointImpactEffect() const { return PointImpactEffect; }
};
