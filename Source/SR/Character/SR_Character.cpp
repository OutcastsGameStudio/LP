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
#include "Components/Dash/SR_DashComponent.h"
#include "Components/Energy Component/SR_EnergyComponent.h"
#include "Components/Slide/SR_SlideComponent.h"

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

	SlideComponent = CreateDefaultSubobject<USR_SlideComponent>(TEXT("SlideComponent"));

	// Set the dash component to the character
	DashComponent = CreateDefaultSubobject<USR_DashComponent>(TEXT("DashComponent"));

	// set the energy component to the character
	EnergyComponent = CreateDefaultSubobject<USR_EnergyComponent>(TEXT("EnergyComponent"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASR_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForLedgeGrab();
	ClimbUp();
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

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASR_Character::Look);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ASR_Character::Dash);

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
	if(bIsHanging) return;
	Cast<USR_CharacterMovementComponent>(GetCharacterMovement())->StopWallJump();
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


void ASR_Character::SetCharacterMovementCustomMode(USR_CharacterMovementComponent::CustomMode NewCustomMode)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Custom, NewCustomMode);
}

void ASR_Character::CheckForLedgeGrab()
{
	USR_CharacterMovementComponent* CharacterMovementComponent = Cast<USR_CharacterMovementComponent>(GetCharacterMovement());
	if (bIsHanging || GetCharacterMovement()->IsMovingOnGround() || CharacterMovementComponent->IsWallRunning())
		return;

	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
    
	FHitResult WallHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHitWall = GetWorld()->LineTraceSingleByChannel(WallHit, 
	   Start, 
	   Start + Forward * LedgeGrabReachDistance,
	   ECC_Visibility, 
	   QueryParams);
	
	if(bHitWall)
	{
		FVector EdgeCheckStart = WallHit.ImpactPoint 
			+ FVector(0, 0, LedgeGrabHeight);
	
		FVector VerticalFrontEnd = EdgeCheckStart 
		   + Forward * 60.0f ;

		FVector EdgeCheckEnd = VerticalFrontEnd  // Distance vers l'avant
	   - FVector(0, 0, 100.0f);
	
		
		FHitResult EdgeHit; // if we hit a face of the wall
		bool bFoundEdge = GetWorld()->LineTraceSingleByChannel(EdgeHit,
			EdgeCheckStart,
			EdgeCheckEnd,
			ECC_Visibility,
			QueryParams);

		FHitResult VerticalHit; // if we hit a vertical wall
		bool bFoundVerticalWall = GetWorld()->LineTraceSingleByChannel(VerticalHit,
			EdgeCheckStart,
			VerticalFrontEnd,
			ECC_Visibility,
			QueryParams);

		if (bFoundEdge && !bFoundVerticalWall) // we hit a face of the wall but not a vertical wall
		{
			LedgeLocation = WallHit.ImpactPoint 
				- Forward * 30.0f 
				+ FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
            
			bIsHanging = true;
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCharacterMovement()->StopMovementImmediately();
		}
	}
}

void ASR_Character::ClimbUp()
{
	if (!bIsHanging)
		return;

	FVector TargetLocation = LedgeLocation;
    
	SetActorLocation(FMath::VInterpTo(
		GetActorLocation(),
		TargetLocation,
		GetWorld()->GetDeltaSeconds(),
		ClimbUpSpeed
	));

	// Une fois en haut, reprendre le mouvement normal
	if (FVector::Distance(GetActorLocation(), TargetLocation) < 10.0f)
	{
		bIsHanging = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
void ASR_Character::Dash(const FInputActionValue& Value)
{
		FVector CharacterForward = GetActorForwardVector();
		FVector CharacterRight = GetActorRightVector();

		//if the character is not moving then dash in the direction of the character forward vector
		if (Value.Get<FVector2D>().Size() == 0)
		{
			DashComponent->DashDirection = CharacterForward;
		}
		else
		{
			DashComponent->DashDirection = CharacterForward * Value.Get<FVector2D>().X + CharacterRight * Value.Get<FVector2D>().Y;
		}		
		DashComponent->Dash();	
}

void ASR_Character::Slide()
{
	SlideComponent->CapsuleComponent = GetCapsuleComponent();
	SlideComponent->MeshComponent = GetMesh();
	SlideComponent->CharacterMovement = GetCharacterMovement();
	SlideComponent->StartSlide();
}

void ASR_Character::StopSlide()
{
	SlideComponent->StopSlide();
}

void ASR_Character::StartCrouch()
{
	const bool bIsCurrentlyCrouching = isCrouching || SlideComponent->bIsCrouching;
	const bool bNewCrouchState = !bIsCurrentlyCrouching;
    
	isCrouching = bNewCrouchState;
	SlideComponent->bIsCrouching = bNewCrouchState;
    
	bNewCrouchState ? Crouch() : UnCrouch();
}
