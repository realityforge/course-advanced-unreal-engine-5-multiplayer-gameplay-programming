#include "GameInstances/AG_GameInstance.h"
#include "AbilitySystemGlobals.h"

void UAG_GameInstance::Init()
{
    Super::Init();

    // load global data tables and tags
    UAbilitySystemGlobals::Get().InitGlobalData();
}
