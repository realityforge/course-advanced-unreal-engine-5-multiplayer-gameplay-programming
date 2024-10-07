#include "ActionGameCharacter.h"
#include "AbilitySystem/Attributes/AG_AttributeSetBase.h"
#include "AbilitySystem/Components/AG_AbilitySystemComponent.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
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
#include "Net/UnrealNetwork.h"

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
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom,
                                  USpringArmComponent::SocketName); // Attach the camera to the end of the
                                                                    // boom and let the boom adjust to match
                                                                    // the controller orientation
    FollowCamera->bUsePawnControlRotation = false;                  // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component
    // (inherited from Character) are set in the derived blueprint asset named
    // ThirdPersonCharacter (to avoid direct content references in C++)

    AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    // replicate minimal gameplay effect info to simulated proxies but full info to owners and autonomous proxies
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));
}

void AActionGameCharacter::BeginPlay()
{
    // Call the base class
    Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {

        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

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

void AActionGameCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AActionGameCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AActionGameCharacter::GiveAbilities()
{
    if (HasAuthority())
    {
        check(AbilitySystemComponent);
        for (const auto Ability : CharacterData.Abilities)
        {
            AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));
        }
    }
}

void AActionGameCharacter::ApplyStartupEffects()
{
    if (HasAuthority())
    {
        FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        for (const auto Effect : CharacterData.Effects)
        {
            // ReSharper disable once CppExpressionWithoutSideEffects
            ApplyGameplayEffectToSelf(Effect, EffectContext);
        }
    }
}

void AActionGameCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // serverside initialization of ASC, client initialized in OnRep_PlayerState
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    GiveAbilities();
    ApplyStartupEffects();
}

void AActionGameCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // clientside initialization of ASC, server initialized in in PossessedBy
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool AActionGameCharacter::ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect,
                                                     const FGameplayEffectContextHandle& InEffectContext) const
{
    if (Effect.Get())
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
        if (SpecHandle.IsValid())
        {
            const auto EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            return EffectHandle.WasSuccessfullyApplied();
        }
    }
    return false;
}

UAbilitySystemComponent* AActionGameCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

FCharacterData AActionGameCharacter::GetCharacterData() const
{
    return CharacterData;
}

void AActionGameCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
    CharacterData = InCharacterData;
    InitFromCharacterData(InCharacterData, false);
}

void AActionGameCharacter::OnRep_CharacterData()
{
    InitFromCharacterData(CharacterData, true);
}

void AActionGameCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication) {}

void AActionGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AActionGameCharacter, CharacterData);
}

void AActionGameCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (IsValid(CharacterDataAsset))
    {
        SetCharacterData(CharacterDataAsset->CharacterData);
    }
}
