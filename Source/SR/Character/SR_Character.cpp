// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Climb/SR_ClimbComponent.h"
#include "Components/ContextState/SR_ContextStateComponent.h"
#include "Components/Dash/SR_DashComponent.h"
#include "Components/Energy Component/SR_EnergyComponent.h"
#include "Components/SR_CharacterMovementComponent.h"
#include "Components/Slide/SR_SlideComponent.h"
#include "Components/WallJump/SR_WallJumpComponent.h"
#include "Components/WallRun/SR_WallRunComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "SR/GameplayObjects/PanelControlSystem/SR_PanelControl.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ASR_Character::ASR_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be
	// tweaked in the Character Blueprint instead of recompiling to adjust them
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component
	// (inherited from Character) are set in the derived blueprint asset named
	// ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASR_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASR_Character::SetLedgeGrabHeight(float NewLedgeGrabHeight) { LedgeGrabHeight = NewLedgeGrabHeight; }

float ASR_Character::GetLedgeGrabHeight() const { return LedgeGrabHeight; }

void ASR_Character::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	m_CharacterMovementComponent = Cast<USR_CharacterMovementComponent>(GetCharacterMovement());
}

// Method by state core components used to retrieve State component from the
// character casted as ISR_STATE
ISR_State *ASR_Character::GetState(MotionState StateName) const
{
	switch (StateName)
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
	switch (CurrentState)
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

void ASR_Character::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASR_Character::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASR_Character::StopWallJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASR_Character::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASR_Character::Move);

		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Started, this, &ASR_Character::MoveForward);
		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Completed, this,
		                                   &ASR_Character::StopMoveForward);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASR_Character::Look);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ASR_Character::OnDashPressed);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ASR_Character::OnDashReleased);

		// Slide
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ASR_Character::OnSlidePressed);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Completed, this,
		                                   &ASR_Character::OnSlideReleased);

		// interact
		EnhancedInputComponent->BindAction(UInteractAction, ETriggerEvent::Started, this,
		                                   &ASR_Character::ActivatePanel);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT("'%s' Failed to find an Enhanced Input Component! This template "
			       "is built to use the Enhanced Input system. If you intend to use "
			       "the legacy system, then you will need to update this C++ file."),
		       *GetNameSafe(this));
	}
}

void ASR_Character::Move(const FInputActionValue &Value)
{
	if (bBlockMovementInput)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASR_Character::StopWallJump() { FOnJumpInputPressed.Broadcast(); }

void ASR_Character::Look(const FInputActionValue &Value)
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
	if (ContextStateComponent->GetCurrentMotionState() == MotionState::CLIMB)
	{
		ContextStateComponent->TransitionState(MotionState::NONE);
		// On vérifie si on est pas sur le sol après avoir terminé le climb
		if (!GetCharacterMovement()->IsMovingOnGround())
		{
			// Si on n'est pas sur le sol, on passe en mode falling
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}
		return;
	}
	if (CurrentState == MotionState::WALL_JUMP)
	{
		return;
	}
	Super::Jump();
}

void ASR_Character::MoveForward() { OnMoveForwardInputPressed.Broadcast(); }

void ASR_Character::StopMoveForward() { OnMoveForwardInputReleased.Broadcast(); }

void ASR_Character::SetCharacterMovementCustomMode(USR_CharacterMovementComponent::CustomMode NewCustomMode)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Custom, NewCustomMode);
}

void ASR_Character::OnSlidePressed() { FOnSlideInputPressed.Broadcast(); }

void ASR_Character::OnSlideReleased() { FOnSlideInputReleased.Broadcast(); }

void ASR_Character::OnDashPressed(const FInputActionValue &Value) { OnDashInputPressed.Broadcast(); }

void ASR_Character::OnDashReleased(const FInputActionValue &Value) { OnDashInputReleased.Broadcast(); }

void ASR_Character::ActivatePanel()
{
	if (!bPlatformMoving)
	{
		TArray<AActor *> FoundPanels;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASR_PanelControl::StaticClass(), FoundPanels);

		float ClosestDistance = MAX_FLT;
		NearestPanel = nullptr;

		for (AActor *Panel : FoundPanels)
		{
			ASR_PanelControl *PanelControl = Cast<ASR_PanelControl>(Panel);
			if (PanelControl)
			{
				float Distance = FVector::Dist(GetActorLocation(), PanelControl->GetActorLocation());
				if (Distance < ClosestDistance && Distance <= PanelControl->ActivationDistance)
				{
					ClosestDistance = Distance;
					NearestPanel = PanelControl;
				}
			}
		}

		if (NearestPanel)
		{
			NearestPanel->TryActivatePanel();
		}
	}
}
