#pragma once

#include "ActionGameTypes.h"
#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "AG_AnimInstance.generated.h"

UCLASS()
class ACTIONGAME_API UAG_AnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    const FCharacterAnimData* GetCharacterAnimData() const;

protected:
    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UBlendSpace* GetLocomotionBlendSpace() const;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetIdleAnimation() const;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UBlendSpace* GetCrouchingLocomotionBlendSpace() const;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetCrouchingIdleAnimation() const;
};
