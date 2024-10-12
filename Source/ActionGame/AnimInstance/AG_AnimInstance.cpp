#include "AnimInstance/AG_AnimInstance.h"
#include "ActionGameCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"

const FCharacterAnimData* UAG_AnimInstance::GetCharacterAnimData() const
{
    if (const auto ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor()))
    {
        if (const auto CharacterAnimDataAsset = ActionGameCharacter->GetCharacterData().CharacterAnimData)
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

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
    const auto AnimData = GetCharacterAnimData();
    return AnimData ? AnimData->MovementBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
    const auto AnimData = GetCharacterAnimData();
    return AnimData ? AnimData->IdleAnimation : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchingLocomotionBlendSpace() const
{
    const auto AnimData = GetCharacterAnimData();
    return AnimData ? AnimData->CrouchingBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchingIdleAnimation() const
{
    const auto AnimData = GetCharacterAnimData();
    return AnimData ? AnimData->CrouchingIdleAnimation : nullptr;
}
