// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_DashComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/Motion/SR_MotionController.h"
#include "SR/Character/SR_Character.h"

USR_DashComponent::USR_DashComponent() { PrimaryComponentTick.bCanEverTick = true; }

void USR_DashComponent::BeginPlay()
{
	Super::BeginPlay();
	// Get the character movement component and the owner character
	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();

	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_DashComponent::BeginPlay()"));
		return;
	}

	OwnerCharacter->OnDashInputPressed.AddDynamic(this, &USR_DashComponent::Dash);
	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_DashComponent::LeaveState);

	OriginalGroundFriction = CharacterMovement->GroundFriction;

	PreviousMotionState = ContextStateComponent->GetCurrentMotionState();
	OriginalGroundFriction = CharacterMovement->IsMovingOnGround();
}

void USR_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									  FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bIsInAir = CharacterMovement ? !CharacterMovement->IsMovingOnGround() : false;

	const MotionState CurrentMotionState = ContextStateComponent->GetCurrentMotionState();

	if (CurrentMotionState != PreviousMotionState)
	{
		if (CurrentMotionState == MotionState::WALL_RUN)
		{
			ResetAllCooldowns();
		}
		PreviousMotionState = CurrentMotionState;
	}

	const bool bIsOnGround = !bIsInAir;
	if (bIsOnGround && !bPreviousGroundState)
	{
		ResetAllCooldowns();
	}
	bPreviousGroundState = bIsOnGround;

	// Cooldown update for air dash
	if (!bCanDashInAir)
	{
		CurrentCooldownTimeInAir += DeltaTime;
		if (CurrentCooldownTimeInAir >= DashCooldownInAir)
		{
			bCanDashInAir = true;
			CurrentCooldownTimeInAir = 0.0f;
		}
	}

	// Cooldown update for ground dash
	if (!bCanDashOnGround)
	{
		CurrentCooldownTimeOnGround += DeltaTime;
		if (CurrentCooldownTimeOnGround >= DashCooldownOnGround)
		{
			bCanDashOnGround = true;
			CurrentCooldownTimeOnGround = 0.0f;
		}
	}
}

void USR_DashComponent::ResetAllCooldowns()
{
	bCanDashInAir = true;
	CurrentCooldownTimeInAir = 0.0f;

	bCanDashOnGround = true;
	CurrentCooldownTimeOnGround = 0.0f;
}

void USR_DashComponent::Dash()
{
	if (!CharacterMovement || !OwnerCharacter)
	{
		return;
	}

	bool bIsInAir = !CharacterMovement->IsMovingOnGround();

	// Check the appropriate cooldown based on whether the character is in the air
	// or on the ground
	if (bIsInAir && !bCanDashInAir)
	{
		// Cannot dash in air during cooldown
		return;
	}
	if (!bIsInAir && !bCanDashOnGround)
	{
		// Cannot dash on ground during cooldown
		return;
	}

	// Transition to dash state
	ContextStateComponent->TransitionState(MotionState::DASH);
}

void USR_DashComponent::UpdateState()
{
	bool bIsInAir = !CharacterMovement->IsMovingOnGround();

	FRootMotionRequest Request;
	Request.MovementName = FName("Dash");
	if (bIsInAir)
	{
		Request.Strength = DashSpeedInAir;
	}
	else
	{
		Request.Strength = DashSpeedOnGround;
	}
	Request.Duration = 0.1f;
	Request.Direction = OwnerCharacter->GetActorForwardVector();
	Request.bIsAdditive = false;
	Request.Priority = ERootMotionPriority::High;
	Request.bEnableGravity = false;
	Request.VelocityOnFinish = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;

	CurrentRootMotionID = MotionController->ApplyRootMotion(Request);

	// disable ground friction
	if (!bIsInAir)
	{
		CharacterMovement->GroundFriction = 0.0f;
	}
}

void USR_DashComponent::EnterState(void *data)
{
	if (bIsStateActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("USR_DashComponent::EnterState() - State is already active"));
		return;
	}

	UpdateState();
	bIsStateActive = true;
	OnDashStarted.Broadcast();
}

void USR_DashComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if (!bForced && rootMotionId != CurrentRootMotionID)
	{
		return;
	}

	if (!ContextStateComponent)
		UE_LOG(LogTemp, Error, TEXT("Failed to load ContextState in USR_DashComponent::LeaveState()"));

	// Apply the appropriate cooldown based on whether the character is in the air
	// or on the ground
	bool bIsInAir = !CharacterMovement->IsMovingOnGround();
	if (bIsInAir)
	{
		bCanDashInAir = false;
		CurrentCooldownTimeInAir = 0.0f;
	}
	else
	{
		bCanDashOnGround = false;
		CurrentCooldownTimeOnGround = 0.0f;
	}

	// enable ground friction
	if (CharacterMovement)
	{
		CharacterMovement->GroundFriction = OriginalGroundFriction;
	}

	bIsStateActive = false;
	ContextStateComponent->TransitionState(MotionState::NONE, bForced);

	OnDashEnded.Broadcast();
}

// used to check if the state can be activated
bool USR_DashComponent::LookAheadQuery()
{
	if (bIsStateActive)
	{
		return false;
	}

	bool bIsInAir = !CharacterMovement->IsMovingOnGround();

	// Return true only if the appropriate cooldown allows dashing
	return bIsInAir ? bCanDashInAir : bCanDashOnGround;
}

FName USR_DashComponent::GetStateName() const { return FName("Dash"); }

int32 USR_DashComponent::GetStatePriority() const
{
	return 0; // max priority
}

bool USR_DashComponent::IsStateActive() const { return bIsStateActive; }
