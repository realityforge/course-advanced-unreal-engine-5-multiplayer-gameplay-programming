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
    FName ItemName{ NAME_None };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<class AItemActor> ItemActorClass{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachmentSocket{ NAME_None };

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

    /** The maximum number of entries that can be included in each inventory slot. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxStackCount{ 1 };
};

UCLASS(Abstract)
class UWeaponStaticData : public UItemStaticData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> DamageEffect{ nullptr };

    // One of Skeletal or Static mesh will be used. We will try one and then the other in the actor

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<USkeletalMesh> SkeletalMesh{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMesh> StaticMesh{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimMontage> AttackMontage{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FireRate{ 0.f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDamage{ 0.f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<USoundBase> AttackSound{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag AmmoTag{ FGameplayTag::EmptyTag };
};

UCLASS(Abstract)
class UAmmoItemStaticData : public UItemStaticData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMesh> StaticMesh{ nullptr };
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
    None UMETA(DisplayName = "None"),
    Equipped UMETA(DisplayName = "Equipped"),
    Dropped UMETA(DisplayName = "Dropped")
};

UENUM(BlueprintType)
enum class EMovementDirectionType : uint8
{
    OrientToMovement UMETA(DisplayName = "Orient To Movement"),
    Strafe UMETA(DisplayName = "Strafe")
};

UCLASS(BlueprintType, Blueprintable, Abstract)
class UProjectileStaticData : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDamage{ 0.f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float DamageRadius{ 0.f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float GravityMultiplier{ 1.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InitialSpeed{ 3000.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxSpeed{ 3000.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMesh> ProjectileMesh{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> DamageEffects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryTypes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UNiagaraSystem> ImpactEffect{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<USoundBase> ImpactSound{ nullptr };
};
