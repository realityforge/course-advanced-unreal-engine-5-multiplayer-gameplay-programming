#include "ActionGameCharacter.h"
#include "AbilitySystem/Attributes/AG_AttributeSetBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"
#include "ActorComponents/FootstepsComponent.h"
#include "Aeon/AbilitySystem/AeonAbilitySet.h"
#include "Aeon/AbilitySystem/AeonAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayEffectTypes.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AActionGameCharacter

AActionGameCharacter::AActionGameCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(CharacterMovementComponentName))
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be
    // tweaked in the Character Blueprint instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom,
                                  USpringArmComponent::SocketName); // Attach the camera to the end of the
    // boom and let the boom adjust to match
    // the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component
    // (inherited from Character) are set in the derived blueprint asset named
    // ThirdPersonCharacter (to avoid direct content references in C++)

    AbilitySystemComponent = CreateDefaultSubobject<UAeonAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    // replicate minimal gameplay effect info to simulated proxies but full info to owners and autonomous proxies
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute())
        .AddUObject(this, &AActionGameCharacter::OnMaxMovementSpeedChanged);

    FootstepsComponent = CreateDefaultSubobject<UFootstepsComponent>("FootstepsComponent");
    MotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>("MotionWarpingComponent");
}

void AActionGameCharacter::PerformJump()
{
    // We do not call super here and implement jump to the parent class
    // instead send event to gameplay ability system and have it do the jumping
    // FGameplayEventData Payload;
    // Payload.Instigator = this;
    // Payload.EventTag = JumpEventTag;
    //
    // UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpEventTag, Payload);

    // New style
    GetAGCharacterMovementComponent()->TryTraversal(GetAeonAbilitySystemComponent_Fast());
}

void AActionGameCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (AbilitySystemComponent)
    {
        // We have landed so remove all effects that are only applicable while in the air
        AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
    }
}

void AActionGameCharacter::OnStartCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
    if (CrouchStateEffectClass)
    {
        if (const auto ASC = AbilitySystemComponent.Get())
        {
            const auto EffectContext = ASC->MakeEffectContext();
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto EffectSpecHandle = ASC->MakeOutgoingSpec(CrouchStateEffectClass, 1, EffectContext);
            if (!EffectSpecHandle.IsValid())
            {
                ABILITY_LOG(Warning, TEXT("Failed to apply CrouchStateEffect"));
            }
        }
    }
    else
    {
        ABILITY_LOG(Warning, TEXT("CrouchStateEffect not specified"));
    }
}

void AActionGameCharacter::OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
    if (CrouchStateEffectClass)
    {
        if (const auto ASC = AbilitySystemComponent.Get())
        {
            ASC->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffectClass, nullptr);
        }
    }
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AActionGameCharacter::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
    GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Add Input Mapping Context
    if (const auto PlayerController = Cast<APlayerController>(GetController()))
    {
        const auto LocalPlayer = PlayerController->GetLocalPlayer();
        if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Set up action bindings
    if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction,
                                           ETriggerEvent::Started,
                                           this,
                                           &AActionGameCharacter::PerformJump);

        if (CrouchAction)
        {
            EnhancedInputComponent->BindAction(CrouchAction,
                                               ETriggerEvent::Started,
                                               this,
                                               &AActionGameCharacter::OnCrouchStarted);
            EnhancedInputComponent->BindAction(CrouchAction,
                                               ETriggerEvent::Completed,
                                               this,
                                               &AActionGameCharacter::OnCrouchEnded);
        }

        if (SprintAction)
        {
            EnhancedInputComponent->BindAction(SprintAction,
                                               ETriggerEvent::Started,
                                               this,
                                               &AActionGameCharacter::OnSprintStarted);
            EnhancedInputComponent->BindAction(SprintAction,
                                               ETriggerEvent::Completed,
                                               this,
                                               &AActionGameCharacter::OnSprintEnded);
        }

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Look);
    }
    else
    {
        UE_LOG(LogTemplateCharacter,
               Error,
               TEXT("'%s' Failed to find an Enhanced Input component! This template "
                    "is built to use the Enhanced Input system. If you intend to use "
                    "the legacy system, then you will need to update this C++ file."),
               *GetNameSafe(this));
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AActionGameCharacter::OnCrouchStarted()
{
    if (const auto ASC = AbilitySystemComponent)
    {
        ASC->TryActivateAbilitiesByTag(CrouchTags, true);
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AActionGameCharacter::OnCrouchEnded()
{
    if (const auto ASC = AbilitySystemComponent)
    {
        ASC->CancelAbilities(&CrouchTags);
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AActionGameCharacter::OnSprintStarted()
{
    if (const auto ASC = AbilitySystemComponent)
    {
        ASC->TryActivateAbilitiesByTag(SprintTags, true);
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AActionGameCharacter::OnSprintEnded()
{
    if (const auto ASC = AbilitySystemComponent)
    {
        ASC->CancelAbilities(&SprintTags);
    }
}

void AActionGameCharacter::Move(const FInputActionValue& Value)
{
    if (Controller)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get the forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get the right vector
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement
        const FVector2D MovementVector = Value.Get<FVector2D>();
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AActionGameCharacter::Look(const FInputActionValue& Value)
{
    if (Controller)
    {
        // add yaw and pitch input to controller
        const FVector2D LookAxisVector = Value.Get<FVector2D>();
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AActionGameCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // serverside initialization of ASC, client initialized in OnRep_PlayerState
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    if (AbilitySet)
    {
        AbilitySet->GiveToAbilitySystem(AbilitySystemComponent);
        AttributeSet = CastChecked<UAG_AttributeSetBase>(
            AbilitySystemComponent->GetAttributeSet(UAG_AttributeSetBase::StaticClass()));
    }
}

void AActionGameCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // clientside initialization of ASC, server initialized in PossessedBy
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AActionGameCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UCharacterAnimDataAsset* AActionGameCharacter::GetCharacterAnimDataAsset() const
{
    return CharacterAnimDataAsset;
}
