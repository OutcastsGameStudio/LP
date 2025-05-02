// Fill out your copyright notice in the Description page of Project Settings.
#include "SR_ClimbComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"


// Sets default values for this component's properties
USR_ClimbComponent::USR_ClimbComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


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

	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_ClimbComponent::LeaveState);

}

// Called every frame
void USR_ClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CheckForLedgeGrab();
	UpdateState();
	// ...
}

void USR_ClimbComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	b_IsActive = false;
    
	// Vérifier si le personnage est sur le sol après avoir terminé le climb
	if (OwnerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		// S'il est sur le sol, passer en mode walking
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		// Sinon, il doit tomber
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
    
	// Transition vers l'état NONE
	ContextStateComponent->TransitionState(MotionState::NONE);
}

void USR_ClimbComponent::EnterState(void* data)
{
	b_IsActive = true;
	StartLocation = OwnerCharacter->GetActorLocation();
}

bool USR_ClimbComponent::LookAheadQuery()
{
	return b_IsActive || CharacterMovement->IsMovingOnGround() || ContextStateComponent->GetCurrentMotionState() == MotionState::WALL_RUN;
}

void USR_ClimbComponent::UpdateState()
{
    if (!b_IsActive)
        return;

    UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
    if (!MovementComp)
        return;
    
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    
    bool bIsOnGround = MovementComp->IsMovingOnGround();

    // if already on the ground, stop climbing and high enough from the start location
    if (bIsOnGround && CurrentLocation.Z > StartLocation.Z + 50.0f)
    {
        b_IsActive = false;
    	
    	// prevent glitching
        MovementComp->Velocity = FVector::ZeroVector;
        MovementComp->SetMovementMode(MOVE_Walking);
        
        ContextStateComponent->TransitionState(MotionState::NONE);
        return;
    }
    
    FVector HorizontalCurrent(CurrentLocation.X, CurrentLocation.Y, 0);
    FVector HorizontalTarget(LedgeLocation.X, LedgeLocation.Y, 0);
    float HorizontalDistance = FVector::Distance(HorizontalCurrent, HorizontalTarget);
    
    // If we are above the ledge and close enough horizontally, we can stop climbing
    bool bIsAboveLedge = CurrentLocation.Z > (LedgeLocation.Z + 20.0f);
    bool bIsCloseHorizontally = HorizontalDistance < ReachDistance;
    
    if (bIsAboveLedge && bIsCloseHorizontally)
    {
        b_IsActive = false;

        // Ajouter une légère impulsion vers l'avant lorsqu'on atteint le haut
        FVector ForwardImpulse = OwnerCharacter->GetActorForwardVector() * ForwardImpulseStrength;
        MovementComp->Velocity = ForwardImpulse;
        MovementComp->SetMovementMode(MOVE_Falling);
        
        ContextStateComponent->TransitionState(MotionState::NONE);
        return;
    }
    
    FVector Direction;

    // STEP 1: Go UP while we are below the ledge
    if (CurrentLocation.Z < LedgeLocation.Z + 150)
    {
        Direction = FVector(0, 0, 1);
    }
    // STEP 2: Go FORWARD as we are above the ledge (with a slight upward component to maintain height)
    else
    {
        Direction = FVector(LedgeLocation.X - CurrentLocation.X, 
                           LedgeLocation.Y - CurrentLocation.Y, 
                           0.2f).GetSafeNormal();
    }
    
    float Speed = ClimbUpSpeed;
    FVector DesiredMovement = Direction * Speed;
    
    // Prevent strange glitching by preventing the character to move and other forces to apply
    MovementComp->Velocity = FVector::ZeroVector;
    
    FHitResult Hit;
    MovementComp->SafeMoveUpdatedComponent(
        DesiredMovement,
        OwnerCharacter->GetActorRotation(),
        false,
        Hit,
        ETeleportType::None
    );
}
FName USR_ClimbComponent::GetStateName() const 
{
	return FName("Climb");
}

int32 USR_ClimbComponent::GetStatePriority() const
{
	return 0;
}

bool USR_ClimbComponent::IsStateActive() const 
{
	return b_IsActive;
}

void USR_ClimbComponent::CheckForLedgeGrab()
{
	if (b_IsActive || CharacterMovement->IsMovingOnGround() || ContextStateComponent->GetCurrentMotionState() == MotionState::WALL_RUN)
		return;
	
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector Forward = OwnerCharacter->GetActorForwardVector();
    
	FHitResult WallHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	
	bool bHitWall = GetWorld()->LineTraceSingleByChannel(WallHit, 
	   Start, 
	   Start + Forward * LedgeGrabReachDistance,
	   ECC_Visibility, 
	   QueryParams);
	
	if(bHitWall)
	{
		FVector EdgeCheckStart = WallHit.ImpactPoint 
			+ FVector(0, 0, LedgeGrabHeight * 3);  // multiply LedgedGrabHeight by 3 to get greater lattitude for the ledge grab
	
		FVector VerticalFrontEnd = EdgeCheckStart 
		   + Forward * 10.0f ;
	
		FVector EdgeCheckEnd = VerticalFrontEnd  // Distance vers le bas
			- FVector(0, 0, LedgeGrabHeight * 3);
	
		FHitResult EdgeHit; // if we hit a face of the wall
		bool bFoundEdge = GetWorld()->LineTraceSingleByChannel(EdgeHit,
			VerticalFrontEnd,
			EdgeCheckEnd,
			ECC_Visibility,
			QueryParams);
	
		if (bFoundEdge) // we hit a wall
		{
			auto distanceZFromPlayer = FMath::Abs(EdgeHit.ImpactPoint.Z - OwnerCharacter->GetActorLocation().Z); 
			if(distanceZFromPlayer < LedgeGrabHeight)
			{
				LedgeLocation = EdgeHit.ImpactPoint;
				ContextStateComponent->TransitionState(MotionState::CLIMB);
			}
		}
	}
}