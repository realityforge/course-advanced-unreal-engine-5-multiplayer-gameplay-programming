#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ActionGameGameplayTags
{

    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Inventory_EquipItemActor)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Inventory_DropItem)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Inventory_EquipNext)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Inventory_Unequip)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Started)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Ended)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Enforced_Strafe)
    ACTIONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage)

} // namespace ActionGameGameplayTags
