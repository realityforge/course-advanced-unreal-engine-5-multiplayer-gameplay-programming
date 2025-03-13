#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionGameStatics.generated.h"

class UGameplayEffect;
class UProjectileStaticData;
class AProjectile;
class UItemStaticData;

UCLASS()
class ACTIONGAME_API UActionGameStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ActionGameStatics")
    static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);

    UFUNCTION(BlueprintCallable, Category = "ActionGameStatics", meta = (WorldContext = "WorldContextObject"))
    static void ApplyRadialDamage(UObject* WorldContextObject,
                                  AActor* DamageCauser,
                                  FVector Location,
                                  float Radius,
                                  float DamageAmount,
                                  TArray<TSubclassOf<UGameplayEffect>> DamageEffects,
                                  const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
                                  ETraceTypeQuery TraceType);

    UFUNCTION(BlueprintCallable, Category = "ActionGameStatics", meta = (WorldContext = "WorldContextObject"))
    static AProjectile* LaunchProjectile(UObject* WorldContextObject,
                                         UPARAM(meta = (AllowAbstract = "false"))
                                             TSubclassOf<UProjectileStaticData> ProjectileDataClass,
                                         FTransform Transform,
                                         AActor* Owner,
                                         APawn* Instigator);
};
