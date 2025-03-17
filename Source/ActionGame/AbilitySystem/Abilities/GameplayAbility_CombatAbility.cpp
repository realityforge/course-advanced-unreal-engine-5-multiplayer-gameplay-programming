#include "AbilitySystem/Abilities/GameplayAbility_CombatAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameCharacter.h"
#include "ActionGameGamePlayTags.h"
#include "ActionGameTypes.h"
#include "Camera/CameraComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"

bool UGameplayAbility_CombatAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                                   FGameplayTagContainer* OptionalRelevantTags)
{
    return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags) && HasEnoughAmmo();
}

bool UGameplayAbility_CombatAbility::HasEnoughAmmo() const
{
    if (const auto Data = GetEquippedItemWeaponStaticData())
    {
        if (const auto Inventory = GetInventoryComponent())
        {
            return !Data->AmmoTag.IsValid() || Inventory->GetInventoryTagCount(Data->AmmoTag) > 0;
        }
    }
    return false;
}

void UGameplayAbility_CombatAbility::DecAmmo() const
{
    if (const auto Data = GetEquippedItemWeaponStaticData())
    {
        if (Data->AmmoTag.IsValid())
        {
            if (const auto Inventory = GetInventoryComponent())
            {
                Inventory->RemoveItemWithInventoryTag(Data->AmmoTag, 1);
            }
        }
    }
}

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

bool UGameplayAbility_CombatAbility::GetWeaponToFocusTraceResult(const float TraceDistance,
                                                                 const ETraceTypeQuery TraceType,
                                                                 FHitResult& OutHitResult) const
{
    const auto WeaponActor = GetEquippedWeaponItemActor();
    // ReSharper disable once CppTooWideScopeInitStatement
    const auto Character = GetActionGameCharacterFromActorInfo();
    if (Character && WeaponActor)
    {
        const auto& CameraTransform = Character->GetFollowCamera()->GetComponentTransform();
        const auto FocusTraceEnd =
            CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;
        const TArray ActorsToIgnore{ GetAvatarActorFromActorInfo() };

        // Trace from the camera a certain Trace Distance to find the point at which we should aim
        FHitResult FocusHit;
        UKismetSystemLibrary::LineTraceSingle(GetWorld(),
                                              CameraTransform.GetLocation(),
                                              FocusTraceEnd,
                                              TraceType,
                                              false,
                                              ActorsToIgnore,
                                              EDrawDebugTrace::None,
                                              FocusHit,
                                              true);

        // Next we trace from the muzzle of the weapon to what our camera trace hit
        const auto MuzzleLocation = WeaponActor->GetMuzzleLocation();
        const auto WeaponTraceEnd =
            MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;
        UKismetSystemLibrary::LineTraceSingle(GetWorld(),
                                              MuzzleLocation,
                                              WeaponTraceEnd,
                                              TraceType,
                                              false,
                                              ActorsToIgnore,
                                              EDrawDebugTrace::None,
                                              OutHitResult,
                                              true);

        return OutHitResult.bBlockingHit;
    }
    else
    {
        return false;
    }
}
