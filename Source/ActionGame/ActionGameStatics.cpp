#include "ActionGameStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameGameplayTags.h"
#include "ActionGameTypes.h"
#include "Actors/Projectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/StructuredLog.h"

static TAutoConsoleVariable CVarShowRadialDamage(TEXT("ShowDebugRadialDamage"),
                                                 0,
                                                 TEXT("Draw debug for radial damage.\n"
                                                      "0: off\n"
                                                      "1: on\n"),
                                                 ECVF_Cheat);

const UItemStaticData* UActionGameStatics::GetItemStaticData(const TSubclassOf<UItemStaticData> ItemDataClass)
{
    return IsValid(ItemDataClass) ? ItemDataClass->GetDefaultObject<UItemStaticData>() : nullptr;
}

void UActionGameStatics::ApplyRadialDamage(UObject* WorldContextObject,
                                           AActor* DamageCauser,
                                           const FVector Location,
                                           const float Radius,
                                           const float DamageAmount,
                                           TArray<TSubclassOf<UGameplayEffect>> DamageEffects,
                                           const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
                                           const ETraceTypeQuery TraceType)
{
    TArray<AActor*> OutActors;
    const TArray ActorsToIgnore{ DamageCauser };
    const bool bDebug = CVarShowRadialDamage->GetBool();

    UKismetSystemLibrary::SphereOverlapActors(WorldContextObject,
                                              Location,
                                              Radius,
                                              ObjectTypes,
                                              nullptr,
                                              ActorsToIgnore,
                                              OutActors);
    const auto World = WorldContextObject->GetWorld();
    for (const auto Actor : OutActors)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        FHitResult HitResult;
        if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject,
                                                  Location,
                                                  Actor->GetActorLocation(),
                                                  TraceType,
                                                  true,
                                                  ActorsToIgnore,
                                                  EDrawDebugTrace::None,
                                                  HitResult,
                                                  true))
        {
            const auto Target = HitResult.GetActor();
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
            if (!Target || Target != Actor || !ASC)
            {
                if (bDebug)
                {
                    const auto TargetName = *GetNameSafe(Target);
                    DrawDebugLine(World, Location, Actor->GetActorLocation(), FColor::Red, false, 4.0f, 0, 1.0f);
                    DrawDebugSphere(World, HitResult.Location, 16, 16, FColor::Red, false, 4.0f, 0, 1.0f);
                    DrawDebugString(World, HitResult.Location, TargetName, nullptr, FColor::White, 0, false, 1.0f);
                }
            }
            else
            {
                bool bWasApplied = false;
                auto EffectContext = ASC->MakeEffectContext();
                EffectContext.AddInstigator(DamageCauser, DamageCauser);

                for (const auto DamageEffect : DamageEffects)
                {
                    // ReSharper disable once CppTooWideScopeInitStatement
                    auto Handle = ASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);
                    if (Handle.IsValid())
                    {
                        const auto& Tag = ActionGameGameplayTags::SetByCaller_Damage;
                        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Handle, Tag, -DamageAmount);

                        // ReSharper disable once CppTooWideScopeInitStatement
                        auto GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
                        if (GEHandle.WasSuccessfullyApplied())
                        {
                            bWasApplied = true;
                        }
                    }
                }

                if (bDebug)
                {
                    const auto TargetName = *GetNameSafe(Target);
                    const auto Color = bWasApplied ? FColor::Green : FColor::Red;
                    DrawDebugLine(World, Location, Actor->GetActorLocation(), Color, false, 4.0f, 0, 1.0f);
                    DrawDebugSphere(World, HitResult.Location, 16, 16, Color, false, 4.0f, 0, 1.0f);
                    DrawDebugString(World, HitResult.Location, TargetName, nullptr, FColor::White, 0, false, 1.0f);
                }
            }
        }
        else
        {
            if (bDebug)
            {
                const auto TargetName = *GetNameSafe(HitResult.GetActor());
                DrawDebugLine(World, Location, Actor->GetActorLocation(), FColor::Red, false, 4.0f, 0, 1.0f);
                DrawDebugSphere(World, HitResult.Location, 16, 16, FColor::Red, false, 4.0f, 0, 1.0f);
                DrawDebugString(World, HitResult.Location, TargetName, nullptr, FColor::White, 0, false, 1.0f);
            }
        }
    }
    if (bDebug)
    {
        DrawDebugSphere(World, Location, Radius, 16, FColor::Purple, false, 4.0f, 0, 1.0f);
    }
}

AProjectile* UActionGameStatics::LaunchProjectile(UObject* WorldContextObject,
                                                  const TSubclassOf<UProjectileStaticData> ProjectileDataClass,
                                                  const FTransform Transform,
                                                  AActor* Owner,
                                                  APawn* Instigator)
{
    if (!ProjectileDataClass || ProjectileDataClass->HasAnyClassFlags(CLASS_Abstract))
    {
        UE_LOGFMT(LogTemp, Warning, "Invalid ProjectileDataClass: The class is either null or abstract!");
        return nullptr;
    }
    if (const auto World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
    {
        if (!World->IsNetMode(NM_Client))
        {
            if (const auto Projectile =
                    World->SpawnActorDeferred<AProjectile>(AProjectile::StaticClass(),
                                                           Transform,
                                                           Owner,
                                                           Instigator,
                                                           ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
            {
                Projectile->ProjectileDataClass = ProjectileDataClass;
                Projectile->FinishSpawning(Transform);
                return Projectile;
            }
        }
    }
    return nullptr;
}
