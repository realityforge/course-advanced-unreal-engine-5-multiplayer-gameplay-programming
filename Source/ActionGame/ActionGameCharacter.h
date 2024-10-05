#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
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
    virtual void BeginPlay();

    void InitializeAttributes();
    void GiveAbilities();
    void ApplyStartupEffects();

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    /** Effect used to initialize the attributes at startup. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> DefaultAttributesSet{ nullptr };

    /** Abilities applied at startup. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

    /** Other Effects applied at startup. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
    TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UAG_AbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(Transient)
    TObjectPtr<UAG_AttributeSetBase> AttributeSet;

public:
    bool ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect,
                                   const FGameplayEffectContextHandle& InEffectContext);

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
