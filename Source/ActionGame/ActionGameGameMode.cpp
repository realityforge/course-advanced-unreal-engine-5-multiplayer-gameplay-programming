#include "ActionGameGameMode.h"
#include "PlayerControllers/ActionGamePlayerController.h"
#include "UObject/ConstructorHelpers.h"

AActionGameGameMode::AActionGameGameMode()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (nullptr != PlayerPawnBPClass.Class)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    PlayerControllerClass = AActionGamePlayerController::StaticClass();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AActionGameGameMode::NotifyPlayerDied(AActionGamePlayerController* PlayerController)
{
    if (PlayerController)
    {
        PlayerController->RestartPlayerIn(2.0f);
    }
}
