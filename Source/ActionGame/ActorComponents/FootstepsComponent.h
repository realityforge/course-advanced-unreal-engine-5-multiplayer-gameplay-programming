#pragma once

#include "ActionGameTypes.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "FootstepsComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UFootstepsComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
    FName LeftFootSocketName{ TEXT("foot_l_Socket") };

    UPROPERTY(EditDefaultsOnly)
    FName RightFootSocketName{ TEXT("foot_r_Socket") };

public:
    UFootstepsComponent();

    void HandleFootStep(EFoot Foot);
};
