#include "PlayerControllers/ActionGamePlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameGameMode.h"
#include "ActionGameGamePlayTags.h"

void AActionGamePlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
    {
        DeathStateTagDelegate = ASC->RegisterGameplayTagEvent(ActionGameGameplayTags::State_Dead)
                                    .AddUObject(this, &ThisClass::OnPawnDeathStateChanged);
    }
}

void AActionGamePlayerController::OnUnPossess()
{
    if (DeathStateTagDelegate.IsValid())
    {
        if (const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
        {
            ASC->UnregisterGameplayTagEvent(DeathStateTagDelegate, ActionGameGameplayTags::State_Dead);
        }
    }
    Super::OnUnPossess();
}

AActionGameGameMode* AActionGamePlayerController::GetGameMode() const
{
    const auto World = GetWorld();
    return World ? Cast<AActionGameGameMode>(World->GetAuthGameMode()) : nullptr;
}

void AActionGamePlayerController::OnPawnDeathStateChanged([[maybe_unused]] const FGameplayTag CallbackTag,
                                                          const int32 NewCount)
{
    if (NewCount > 0)
    {
        if (const auto GameMode = GetGameMode())
        {
            GameMode->NotifyPlayerDied(this);
        }

        if (DeathStateTagDelegate.IsValid())
        {
            if (const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
            {
                ASC->UnregisterGameplayTagEvent(DeathStateTagDelegate, ActionGameGameplayTags::State_Dead);
            }
        }
    }
}

void AActionGamePlayerController::RestartPlayer()
{
    const auto World = GetWorld();
    if (const auto GameMode = World ? World->GetAuthGameMode() : nullptr)
    {
        GameMode->RestartPlayer(this);
    }
}

AActionGamePlayerController::AActionGamePlayerController() {}

void AActionGamePlayerController::RestartPlayerIn(const float InTime)
{
    ChangeState(NAME_Spectating);
    GetWorldTimerManager().SetTimer(RestartPlayerTimerHandle, this, &ThisClass::RestartPlayer, InTime);
}
