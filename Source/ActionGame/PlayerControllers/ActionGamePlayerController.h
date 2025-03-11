#pragma once

#include "ActionGameGameMode.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ActionGamePlayerController.generated.h"

struct FGameplayTag;

UCLASS()
class ACTIONGAME_API AActionGamePlayerController : public APlayerController
{
    GENERATED_BODY()

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    AActionGameGameMode* GetGameMode() const;

    UFUNCTION()
    void OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount);

    void RestartPlayer();

    FTimerHandle RestartPlayerTimerHandle;
    FDelegateHandle DeathStateTagDelegate;

public:
    AActionGamePlayerController();

    void RestartPlayerIn(float InTime);
};
