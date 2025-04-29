// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SR_CharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/Climb/SR_ClimbComponent.h"
#include "Components/ContextState/SR_ContextStateComponent.h"
#include "Components/Dash/SR_DashComponent.h"
#include "Components/Energy Component/SR_EnergyComponent.h"
#include "Components/Slide/SR_SlideComponent.h"
#include "Components/WallJump/SR_WallJumpComponent.h"
#include "Components/WallRun/SR_WallRunComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


ASR_Character::ASR_Character()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingFrictionFactor = 200.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 5000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), FName("Camera"));
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// set the energy component to the character
	EnergyComponent = CreateDefaultSubobject<USR_EnergyComponent>(TEXT("EnergyComponent"));

	InteractionComponent = CreateDefaultSubobject<USR_InteractionComponent>(TEXT("InteractionComponent"));
	
	// set the debug component
	DebugComponent = CreateDefaultSubobject<USR_DebugComponent>(TEXT("DebugComponent"));

	// ==== STATE CORE SYSTEM COMPONENTS ====
	ContextStateComponent = CreateDefaultSubobject<USR_ContextStateComponent>(TEXT("ContextStateComponent"));
	MotionController = CreateDefaultSubobject<USR_MotionController>(TEXT("MotionController"));

	// ==== STATE COMPONENTS IMPLEMENTS ISR_STATE INTERFACE ====
	DashComponent = CreateDefaultSubobject<USR_DashComponent>(TEXT("DashComponent"));
	WallRunComponent = CreateDefaultSubobject<USR_WallRunComponent>(TEXT("WallRunComponent"));
	WallJumpComponent = CreateDefaultSubobject<USR_WallJumpComponent>(TEXT("WallJumpComponent"));
	ClimbComponent = CreateDefaultSubobject<USR_ClimbComponent>(TEXT("ClimbComponent"));
	SlideComponent = CreateDefaultSubobject<USR_SlideComponent>(TEXT("SlideComponent"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASR_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotateCharacterWithCamera && Controller)
	{
		// Obtenir la rotation du contrôleur (caméra)
		FRotator ControlRotation = Controller->GetControlRotation();
        
		// Nous voulons seulement la rotation Yaw (horizontale)
		ControlRotation.Pitch = 0.0f;
		ControlRotation.Roll = 0.0f;
        
		// Faire tourner le personnage pour qu'il s'aligne avec la caméra
		SetActorRotation(ControlRotation);
	}
}

void ASR_Character::SetLedgeGrabHeight(float NewLedgeGrabHeight)
{
	LedgeGrabHeight = NewLedgeGrabHeight;
}

float ASR_Character::GetLedgeGrabHeight() const
{
	return LedgeGrabHeight;
}

void ASR_Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	m_CharacterMovementComponent = Cast<USR_CharacterMovementComponent>(GetCharacterMovement());
}

// Method by state core components used to retrieve State component from the character casted as ISR_STATE 
ISR_State* ASR_Character::GetState(MotionState StateName) const
{
	switch(StateName)
	{
		case MotionState::DASH:
			return Cast<ISR_State>(DashComponent);
		case MotionState::WALL_RUN:
			return Cast<ISR_State>(WallRunComponent);
		case MotionState::WALL_JUMP:
			return Cast<ISR_State>(WallJumpComponent);
		case MotionState::CLIMB:
			return Cast<ISR_State>(ClimbComponent);
		case MotionState::SLIDE:
			return Cast<ISR_State>(SlideComponent);
		default:
			return nullptr;
	}
}

FName ASR_Character::GetCurrentStateName()
{
	switch(b_CurrentState)
	{
		case MotionState::DASH:
			return DashComponent->GetStateName();
		case MotionState::WALL_RUN:
			return WallRunComponent->GetStateName();
		case MotionState::WALL_JUMP:
			return WallJumpComponent->GetStateName();
		case MotionState::CLIMB:
			return ClimbComponent->GetStateName();
		case MotionState::SLIDE:
			return SlideComponent->GetStateName();
		default:
			return FName("None");
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASR_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASR_Character::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASR_Character::StopWallJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASR_Character::StopJumping);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ASR_Character::StartCrouch);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASR_Character::Move);

		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Started, this, &ASR_Character::MoveForward);
		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Completed, this, &ASR_Character::StopMoveForward);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASR_Character::Look);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ASR_Character::OnDashPressed);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ASR_Character::OnDashReleased);

		// Slide
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ASR_Character::Slide);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Completed, this, &ASR_Character::StopSlide);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASR_Character::Move(const FInputActionValue& Value)
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

void ASR_Character::StopWallJump()
{
	FOnJumpInputPressed.Broadcast();
}

void ASR_Character::Look(const FInputActionValue& Value)
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


void ASR_Character::Jump()
{
	if(ContextStateComponent->GetCurrentMotionState() == MotionState::CLIMB)
	{
		ContextStateComponent->TransitionState(MotionState::NONE);
		// On vérifie si on est pas sur le sol après avoir terminé le climb
		if(!GetCharacterMovement()->IsMovingOnGround())
		{
			// Si on n'est pas sur le sol, on passe en mode falling
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}
		return;
	}
	if(b_CurrentState == MotionState::WALL_JUMP)
		return;
	Super::Jump();
}

void ASR_Character::MoveForward()
{
	OnMoveForwardInputPressed.Broadcast();
}

void ASR_Character::StopMoveForward()
{
	OnMoveForwardInputReleased.Broadcast();
}

void ASR_Character::SetCharacterMovementCustomMode(USR_CharacterMovementComponent::CustomMode NewCustomMode)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Custom, NewCustomMode);
}

void ASR_Character::Slide()
{
	FOnSlideInputPressed.Broadcast();
}

void ASR_Character::StopSlide()
{
	FOnSlideInputReleased.Broadcast();
}

void ASR_Character::StartCrouch()
{
	FOnCrouchInputPressed.Broadcast();
}

void ASR_Character::StopCrouch()
{
	FOnCrouchInputReleased.Broadcast();
}

void ASR_Character::OnDashPressed(const FInputActionValue& Value)
{
	OnDashInputPressed.Broadcast();
}

void ASR_Character::OnDashReleased(const FInputActionValue& Value)
{
	OnDashInputReleased.Broadcast();
}