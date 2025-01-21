#pragma once

#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Logging/LogMacros.h"
#include "ActionGameCharacter.generated.h"

class UAeonAbilitySet;
class UFootstepsComponent;
class UAbilitySystemComponent;
struct FGameplayEffectContextHandle;
class UGameplayAbility;
struct FActiveGameplayEffectHandle;
class UGameplayEffect;
class UAeonAbilitySystemComponent;
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Footsteps", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFootstepsComponent> FootstepsComponent{ nullptr };

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> SprintAction{ nullptr };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    FGameplayTag JumpEventTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    FGameplayTagContainer InAirTags;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    FGameplayTagContainer CrouchTags;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGameplayEffect> CrouchStateEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    FGameplayTagContainer SprintTags;

public:
    explicit AActionGameCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void PerformJump();
    virtual void Landed(const FHitResult& Hit) override;
    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

    void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);

protected:
    void OnCrouchStarted();
    void OnCrouchEnded();

    void OnSprintStarted();
    void OnSprintEnded();

    FDelegateHandle MaxMovementSpeedChangeDelegateHandle;

    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    UPROPERTY(VisibleDefaultsOnly)
    TObjectPtr<UAeonAbilitySystemComponent> AbilitySystemComponent{ nullptr };

    UPROPERTY(Transient)
    TObjectPtr<UAG_AttributeSetBase const> AttributeSet{ nullptr };

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAeonAbilitySet> AbilitySet{ nullptr };

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCharacterAnimDataAsset> CharacterAnimDataAsset{ nullptr };

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    FORCEINLINE UAeonAbilitySystemComponent* GetAeonAbilitySystemComponent_Fast() const
    {
        return AbilitySystemComponent;
    }

    /** Returns CameraBoom subobject **/
    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    /** Returns UFootstepsComponent subobject **/
    FORCEINLINE UFootstepsComponent* GetFootstepsComponent() const { return FootstepsComponent; }

    UFUNCTION(BlueprintCallable)
    UCharacterAnimDataAsset* GetCharacterAnimDataAsset() const;
};
