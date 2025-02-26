#include "AnimInstance/AG_AnimInstance.h"
#include "ActionGameCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"

const FCharacterAnimData* UAG_AnimInstance::GetCurrentItemAnimData() const
{
    if (const auto ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
    {
        const auto InventoryComponent = ActionGameCharacter->GetInventoryComponent();
        if (const auto ItemInstance = InventoryComponent->GetCurrentItem())
        {
            return &ItemInstance->GetItemStaticData()->CharacterAnimData;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

const FCharacterAnimData* UAG_AnimInstance::GetCharacterAnimData() const
{
    if (const auto ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
    {
        if (const auto CharacterAnimDataAsset = ActionGameCharacter->GetCharacterAnimDataAsset())
        {
            return &CharacterAnimDataAsset->CharacterAnimData;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

#define GET_ANIM_DATA(PropertyName)                             \
    {                                                           \
        if (const auto ItemAnimData = GetCurrentItemAnimData()) \
        {                                                       \
            if (ItemAnimData->PropertyName)                     \
            {                                                   \
                return ItemAnimData->PropertyName;              \
            }                                                   \
        }                                                       \
        const auto AnimData = GetCharacterAnimData();           \
        return AnimData ? AnimData->PropertyName : nullptr;     \
    }

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const { GET_ANIM_DATA(MovementBlendSpace) }

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const { GET_ANIM_DATA(IdleAnimation) }

UBlendSpace* UAG_AnimInstance::GetCrouchingLocomotionBlendSpace() const { GET_ANIM_DATA(CrouchingBlendSpace) }

UAnimSequenceBase* UAG_AnimInstance::GetCrouchingIdleAnimation() const
{
    GET_ANIM_DATA(CrouchingIdleAnimation)
}
