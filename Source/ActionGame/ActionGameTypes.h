#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FCharacterData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS", meta = (AllowAbstract = "false"))
    TArray<TSubclassOf<UGameplayAbility>> Abilities;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS", meta = (AllowAbstract = "false"))
    TArray<TSubclassOf<UGameplayEffect>> Effects;
};
