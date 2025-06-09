// Fill out your copyright notice in the Description page of Project Settings.
#include "SR_ClimbComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USR_ClimbComponent::USR_ClimbComponent() { PrimaryComponentTick.bCanEverTick = true; }

// Called when the game starts
void USR_ClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();

	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_ClimbComponent::BeginPlay()"));
		return;
	}
	OwnerCharacter->OnMoveForwardInputPressed.AddDynamic(this, &USR_ClimbComponent::OnMoveForwardInputPressed);
	OwnerCharacter->OnMoveForwardInputReleased.AddDynamic(this, &USR_ClimbComponent::OnMoveForwardInputReleased);

	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_ClimbComponent::LeaveState);
}

// Called every frame
void USR_ClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									   FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CheckForLedgeGrab();
	UpdateState();
	// ...
}

void USR_ClimbComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	bIsActive = false;
	elapsedTime = 0.f;
	bIsLedgeGrabbed = false;

	if (OwnerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{

		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{

		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}

	ContextStateComponent->TransitionState(MotionState::NONE);
}

void USR_ClimbComponent::OnMoveForwardInputPressed() { bIsMovingForward = true; }

void USR_ClimbComponent::OnMoveForwardInputReleased() { bIsMovingForward = false; }

void USR_ClimbComponent::EnterState(void *data)
{
	bIsActive = true;
	StartLocation = OwnerCharacter->GetActorLocation();
	if(!bIsLedgeGrabbed)
	{
		OnLedgeGrabStarted.Broadcast();
		bIsLedgeGrabbed = true;
	} else
	{
		elapsedTime += GetWorld()->GetDeltaSeconds();
	}
}

bool USR_ClimbComponent::LookAheadQuery()
{
	return bIsActive || CharacterMovement->IsMovingOnGround() ||
		ContextStateComponent->GetCurrentMotionState() == MotionState::WALL_RUN;
}

void USR_ClimbComponent::UpdateState()
{
	if (!bIsActive)
	{
		return;
	}

	UCharacterMovementComponent *MovementComp = OwnerCharacter->GetCharacterMovement();
	if (!MovementComp)
	{
		return;
	}

	FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	bool bIsOnGround = MovementComp->IsMovingOnGround();

	if (bIsOnGround && CurrentLocation.Z > StartLocation.Z + 50.0f)
	{
		bIsActive = false;
		bIsLedgeGrabbed = false;
		elapsedTime = 0.f;

		// prevent glitching
		MovementComp->Velocity = FVector::ZeroVector;
		MovementComp->SetMovementMode(MOVE_Walking);

		ContextStateComponent->TransitionState(MotionState::NONE);
		return;
	}

	FVector HorizontalCurrent(CurrentLocation.X, CurrentLocation.Y, 0);
	FVector HorizontalTarget(LedgeLocation.X, LedgeLocation.Y, 0);
	float HorizontalDistance = FVector::Distance(HorizontalCurrent, HorizontalTarget);

	// If we are above the ledge and close enough horizontally, we can stop
	// climbing
	bool bIsAboveLedge = CurrentLocation.Z > (LedgeLocation.Z + 20.0f);
	bool bIsCloseHorizontally = HorizontalDistance < ReachDistance;

	if (bIsAboveLedge && bIsCloseHorizontally)
	{
		bIsActive = false;
		elapsedTime = 0.f;

		// add an impulse to the character to get him off the ledge
		FVector ForwardImpulse = OwnerCharacter->GetActorForwardVector() * ForwardImpulseStrength;
		OwnerCharacter->LaunchCharacter(ForwardImpulse, true, false);
		MovementComp->SetMovementMode(MOVE_Falling);
		bIsLedgeGrabbed = false;
		ContextStateComponent->TransitionState(MotionState::NONE);
		return;
	}

	FVector Direction;
	float Speed = ClimbUpSpeed;
	// STEP 1: Go UP while we are below the ledge
	if (CurrentLocation.Z < LedgeLocation.Z - 10.f)
	{
		Direction = FVector(0, 0, 1);
	}
	else if (bIsLedgeGrabbed && elapsedTime <= 0.5f)
	{
		// If we are above the ledge and still in the ledge grab state, we can
		// just wait for a bit before moving forward
		Direction = FVector(0, 0, 0);
	}
	// else if (bIsLedgeGrabbed && elapsedTime > 0.5f && elapsedTime <= 2.f)
	// {
	// 	// If we are above the ledge and still in the ledge grab state, we can
	// 	// just wait for a bit before moving forward
	// 	Direction = FVector(0, 0, 1);
	// 	Speed = Speed * 0.5f; // slow down the climb speed to make it
	// 	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
	// 							 TEXT("Waiting before climbing forward!"));
	// }
	// STEP 2: Go FORWARD as we are above the ledge (with a slight upward
	// component to maintain height)
	else
	{

		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green,
								 TEXT("Climbing forward!"));
		Direction = FVector(LedgeLocation.X - CurrentLocation.X, LedgeLocation.Y - CurrentLocation.Y, UpwardValue)
						.GetSafeNormal();
	}

	FVector DesiredMovement = Direction * Speed;

	// Prevent strange glitching by preventing the character to move and other
	// forces to apply
	MovementComp->Velocity = FVector::ZeroVector;

	FHitResult Hit;
	MovementComp->SafeMoveUpdatedComponent(DesiredMovement, OwnerCharacter->GetActorRotation(), false, Hit,
										   ETeleportType::None);
}
FName USR_ClimbComponent::GetStateName() const { return FName("Climb"); }

int32 USR_ClimbComponent::GetStatePriority() const { return 0; }

bool USR_ClimbComponent::IsStateActive() const { return bIsActive; }

void USR_ClimbComponent::CheckForLedgeGrab()
{

	if (bIsActive || CharacterMovement->IsMovingOnGround() ||
		ContextStateComponent->GetCurrentMotionState() == MotionState::WALL_RUN && !bIsMovingForward)
	{
		return;
	}

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector Forward = OwnerCharacter->GetActorForwardVector();

	FHitResult WallHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	bool bHitWall = GetWorld()->LineTraceSingleByChannel(WallHit, Start, Start + Forward * LedgeGrabReachDistance,
														 ECC_Visibility, QueryParams);

	if (bHitWall)
	{
		FVector EdgeCheckStart = WallHit.ImpactPoint +
			FVector(0, 0,
					LedgeGrabHeight * 3); // multiply LedgedGrabHeight by 3 to get
		// greater lattitude for the ledge grab

		FVector VerticalFrontEnd = EdgeCheckStart + Forward * 10.0f;

		FVector EdgeCheckEnd = VerticalFrontEnd - FVector(0, 0, LedgeGrabHeight * 3);

		FHitResult EdgeHit; // if we hit a face of the wall
		bool bFoundEdge =
			GetWorld()->LineTraceSingleByChannel(EdgeHit, VerticalFrontEnd, EdgeCheckEnd, ECC_Visibility, QueryParams);

		if (bFoundEdge) // we hit a wall
		{
			auto distanceZFromPlayer = FMath::Abs(EdgeHit.ImpactPoint.Z - OwnerCharacter->GetActorLocation().Z);
			if (distanceZFromPlayer < LedgeGrabHeight)
			{
				LedgeLocation = EdgeHit.ImpactPoint;
				ContextStateComponent->TransitionState(MotionState::CLIMB);
			}
		}
	}
}
