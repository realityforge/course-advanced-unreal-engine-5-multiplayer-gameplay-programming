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

} // namespace ActionGameGameplayTags
