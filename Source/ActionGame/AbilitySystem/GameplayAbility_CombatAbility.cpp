#include "AbilitySystem/GameplayAbility_CombatAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameGamePlayTags.h"
#include "ActionGameTypes.h"

FGameplayEffectSpecHandle UGameplayAbility_CombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult) const
{
    if (const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
    {
        if (const auto StaticData = GetEquippedItemWeaponStaticData())
        {
            const auto EffectContext = AbilitySystemComponent->MakeEffectContext();
            const auto Handle = AbilitySystemComponent->MakeOutgoingSpec(StaticData->DamageEffect, 1, EffectContext);
            const auto& Tag = ActionGameGameplayTags::SetByCaller_Damage;
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Handle, Tag, -StaticData->BaseDamage);

            return Handle;
        }
    }

    return FGameplayEffectSpecHandle();
}
