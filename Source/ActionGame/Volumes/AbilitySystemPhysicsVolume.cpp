#include "Volumes/AbilitySystemPhysicsVolume.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAbilitySystemPhysicsVolume::AAbilitySystemPhysicsVolume()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemPhysicsVolume::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bDrawDebug)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), GetBounds().BoxExtent, FColor::Red, false, 0, 0, 5);
    }
}

void AAbilitySystemPhysicsVolume::ActorEnteredVolume(AActor* Other)
{
    Super::ActorEnteredVolume(Other);

    if (HasAuthority())
    {
        if (const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
        {
            for (const auto Ability : PermanentAbilitiesToGive)
            {
                ASC->GiveAbility(FGameplayAbilitySpec(Ability));
            }

            EnteredActorsInfoMap.Add(Other);

            for (const auto Ability : OngoingAbilitiesToGive)
            {
                const auto AbilityHandle = ASC->GiveAbility(FGameplayAbilitySpec(Ability));
                EnteredActorsInfoMap[Other].AppliedAbilities.Add(AbilityHandle);
            }

            auto EffectContext = ASC->MakeEffectContext();
            EffectContext.AddInstigator(Other, Other);

            for (const auto Effect : OngoingEffectsToApply)
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                auto SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);
                if (SpecHandle.IsValid())
                {
                    // ReSharper disable once CppTooWideScopeInitStatement
                    auto Handle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                    if (Handle.WasSuccessfullyApplied())
                    {
                        EnteredActorsInfoMap[Other].AppliedEffects.Add(Handle);
                    }
                }
            }

            for (const auto Tag : GameplayEventsToSendOnEnter)
            {
                FGameplayEventData Data;
                Data.EventTag = Tag;
                UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, Data.EventTag, Data);
            }
        }
    }
}

void AAbilitySystemPhysicsVolume::ActorLeavingVolume(AActor* Other)
{
    if (HasAuthority() && EnteredActorsInfoMap.Contains(Other))
    {
        if (const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
        {
            for (const auto Handle : EnteredActorsInfoMap[Other].AppliedEffects)
            {
                ASC->RemoveActiveGameplayEffect(Handle);
            }

            for (const auto Handle : EnteredActorsInfoMap[Other].AppliedAbilities)
            {
                ASC->ClearAbility(Handle);
            }

            EnteredActorsInfoMap.Remove(Other);

            auto EffectContext = ASC->MakeEffectContext();
            EffectContext.AddInstigator(Other, Other);

            for (const auto Effect : OnExitEffectsToApply)
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                auto SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);
                if (SpecHandle.IsValid())
                {
                    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            }

            for (const auto Tag : GameplayEventsToSendOnExit)
            {
                FGameplayEventData EventData;
                EventData.EventTag = Tag;
                UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, Tag, EventData);
            }
        }
    }
}
