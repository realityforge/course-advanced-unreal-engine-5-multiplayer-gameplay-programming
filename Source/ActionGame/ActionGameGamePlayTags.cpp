#include "ActionGameGameplayTags.h"

namespace ActionGameGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Inventory_EquipItemActor,
                                   "Event.Inventory.EquipItemActor",
                                   "Equip Item from ItemActor Event")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Inventory_DropItem, "Event.Inventory.DropItem", "Drop current item Event")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Inventory_EquipNext,
                                   "Event.Inventory.EquipNext",
                                   "Try and Equip Next Item Event")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Inventory_Unequip, "Event.Inventory.Unequip", "Unequip current item Event")

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Attack_Started, "Event.Attack.Started", "Attack button pressed")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Attack_Ended, "Event.Attack.Ended", "Attack button released")

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Aim_Started, "Event.Aim.Started", "Aim button pressed")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Aim_Ended, "Event.Aim.Ended", "Aim button released")

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Enforced_Strafe,
                                   "Movement.Enforced.Strafe",
                                   "Tag is applied to change movement mode to strafing")

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "The Damage applied by the Effect")

} // namespace ActionGameGameplayTags
