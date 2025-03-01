#include "ActionGameGameMode.h"
#include "ActionGameCharacter.h"
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
}
