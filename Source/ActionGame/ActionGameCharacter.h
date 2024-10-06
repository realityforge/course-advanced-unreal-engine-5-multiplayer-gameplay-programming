#pragma once

#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "CoreMinimal.h"
#include "DataAssets/CharacterDataAsset.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ActionGameCharacter.generated.h"

class UAbilitySystemComponent;
struct FGameplayEffectContextHandle;
class UGameplayAbility;
struct FActiveGameplayEffectHandle;
class UGameplayEffect;
class UAG_AbilitySystemComponent;
class UAG_AttributeSetBase;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AActionGameCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

public:
    AActionGameCharacter();

protected:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // To add mapping context
    virtual void BeginPlay() override;

    void GiveAbilities();
    void ApplyStartupEffects();

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UAG_AbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(Transient)
    TObjectPtr<UAG_AttributeSetBase> AttributeSet;

    UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
    FCharacterData CharacterData;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCharacterDataAsset> CharacterDataAsset{ nullptr };

    UFUNCTION()
    void OnRep_CharacterData();

    virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PostInitializeComponents() override;

    bool ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect,
                                   const FGameplayEffectContextHandle& InEffectContext) const;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UFUNCTION(BlueprintCallable)
    FCharacterData GetCharacterData() const;

    UFUNCTION(BlueprintCallable)
    void SetCharacterData(const FCharacterData& InCharacterData);
};
