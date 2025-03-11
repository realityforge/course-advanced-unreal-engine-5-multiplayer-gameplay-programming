#include "ActionGameGameplayTags.h"

/*

+GameplayTagList=(Tag="Ability.Combat.SingleShot",DevComment="")
+GameplayTagList=(Tag="Ability.Death",DevComment="")
+GameplayTagList=(Tag="Ability.Movement.Crouch",DevComment="")
+GameplayTagList=(Tag="Ability.Movement.InAir.Jump",DevComment="")
+GameplayTagList=(Tag="Ability.Movement.Sprint",DevComment="")
+GameplayTagList=(Tag="Ability.Movement.Vault",DevComment="")
+GameplayTagList=(Tag="Ability.Movement.WallRun",DevComment="")
+GameplayTagList=(Tag="Ammo.Pistol",DevComment="")
+GameplayTagList=(Tag="Ammo.Rifle",DevComment="")
+GameplayTagList=(Tag="Attribute.Health",DevComment="")
+GameplayTagList=(Tag="Attribute.Health.Zero",DevComment="")
+GameplayTagList=(Tag="Event.Combat.Aim.Ended",DevComment="")
+GameplayTagList=(Tag="Event.Combat.Aim.Started",DevComment="")
+GameplayTagList=(Tag="Event.Combat.Attack.Ended",DevComment="")
+GameplayTagList=(Tag="Event.Combat.Attack.Started",DevComment="")
+GameplayTagList=(Tag="Event.Combat.Shoot",DevComment="")
+GameplayTagList=(Tag="Event.Movement.Jump",DevComment="")
+GameplayTagList=(Tag="GameplayCue.Burn",DevComment="")
+GameplayTagList=(Tag="GameplayCue.Burn2",DevComment="")
+GameplayTagList=(Tag="Movement.Enforced.Strafe",DevComment="")
+GameplayTagList=(Tag="State.Combat.Aiming",DevComment="")
+GameplayTagList=(Tag="State.Dead",DevComment="")
+GameplayTagList=(Tag="State.InAir.Jumping",DevComment="")
+GameplayTagList=(Tag="State.Movement.Crouch",DevComment="")
+GameplayTagList=(Tag="State.Movement.Sprint",DevComment="")
+GameplayTagList=(Tag="State.Movement.Vault",DevComment="")
+GameplayTagList=(Tag="State.Movement.WallRun",DevComment="")
+GameplayTagList=(Tag="State.Movement.WallRun.Left",DevComment="")
+GameplayTagList=(Tag="State.Movement.WallRun.Right",DevComment="")
+GameplayTagList=(Tag="State.Ragdoll",DevComment="")



 */

namespace ActionGameGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Death, "Ability.Death", "Ability triggered when the actor dies")

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

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Combat_Aiming, "State.Combat.Aiming", "The actor is currently aiming")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Ragdoll, "State.Ragdoll", "The actor is currently in a Ragdoll state")
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "The actor is Dead")

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Zero,
                                   "Attribute.Health.Zero",
                                   "The Health attribute of the character went to zero")

} // namespace ActionGameGameplayTags
