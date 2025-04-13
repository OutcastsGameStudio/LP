// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_ClimbComponent.h"

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
}

bool USR_ClimbComponent::LookAheadQuery()
{
	return b_IsActive || CharacterMovement->IsMovingOnGround() || ContextStateComponent->GetCurrentMotionState() == MotionState::WALL_RUN;
}

void USR_ClimbComponent::UpdateState()
{
	if (!b_IsActive)
		return;
	
	FVector TargetLocation = LedgeLocation;

	//@TODO: maybe move to SafeMoveUpdatedComponent  to handle collisions
	OwnerCharacter->SetActorLocation(FMath::VInterpTo(
		OwnerCharacter->GetActorLocation(),
		TargetLocation,
		GetWorld()->GetDeltaSeconds(),
		ClimbUpSpeed
	));
	
	// Une fois en haut, reprendre le mouvement normal
	if (FVector::Distance(OwnerCharacter->GetActorLocation(), TargetLocation) < 10.0f)
	{
		b_IsActive = false;
		// Vérifier si le personnage est sur le sol après avoir terminé le climb
		if(OwnerCharacter->GetCharacterMovement()->IsMovingOnGround())
		{
			// S'il est sur le sol, passer en mode walking
			OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		else
		{
			// Sinon, il doit tomber
			OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}
		ContextStateComponent->TransitionState(MotionState::NONE);
	}
	
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
				LedgeLocation = WallHit.ImpactPoint 
					- Forward * 30.0f 
					+ FVector(0, 0, OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	            
				ContextStateComponent->TransitionState(MotionState::CLIMB);
			}
		}
	}
}

