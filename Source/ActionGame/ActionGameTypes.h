#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActionGameTypes.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UNiagaraSystem;
class UBlendSpace;
class UAnimSequenceBase;

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

UCLASS(BlueprintType, Blueprintable, Abstract)
class UItemStaticData : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName ItemName{};

    // UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    // TSubclassOf<class AItemActor> ItemActorClass{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachmentSocket{};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCanBeEquipped{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FCharacterAnimData CharacterAnimData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities{};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> GrantedEffects{};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FGameplayTag> InventoryTags{};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxStackCount{ 1 };
};
