#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

class UCharacterAnimDataAsset;
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

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    TObjectPtr<UCharacterAnimDataAsset> CharacterAnimData{ nullptr };
};

USTRUCT(BlueprintType)
struct FCharacterAnimData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UBlendSpace> MovementBlendSpace{ nullptr };

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UBlendSpace> CrouchingBlendSpace{ nullptr };

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UAnimSequenceBase> IdleAnimation{ nullptr };

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UAnimSequenceBase> CrouchingIdleAnimation{ nullptr };
};

UENUM(BlueprintType)
enum class EFoot : uint8
{
    Left,
    Right
};
