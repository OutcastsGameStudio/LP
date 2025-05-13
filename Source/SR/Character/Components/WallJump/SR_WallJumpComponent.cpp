// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_WallJumpComponent.h"


// Sets default values for this component's properties
USR_WallJumpComponent::USR_WallJumpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USR_WallJumpComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();

	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_WallJumpComponent::BeginPlay()"));
	}

	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_WallJumpComponent::LeaveState);
}


// Called every frame
void USR_WallJumpComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USR_WallJumpComponent::EnterState(void* data)
{
	// Réinitialiser les variables à chaque entrée d'état
	m_WallRunDirection = FVector::ZeroVector;
	m_WallNormal = FVector::ZeroVector;
    
	// Récupérer les données de wall run
	FWallJumpData* wallJumpData = static_cast<FWallJumpData*>(data);
	if(wallJumpData)
	{
		m_WallRunDirection = wallJumpData->WallRunDirection;
		m_WallNormal = wallJumpData->WallNormal;
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast data to FWallJumpData"));
	}
	bIsActive = true;
    
	// Appliquer le wall jump
	UpdateState();
}

void USR_WallJumpComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if(!bForced && rootMotionId != m_WallRunMainMotionId) return;
	bIsActive = false;
	ContextStateComponent->TransitionState(MotionState::NONE, bForced);
}

bool USR_WallJumpComponent::LookAheadQuery()
{
	return false;
}

void USR_WallJumpComponent::UpdateState()
{
	FVector JumpDirection = (m_WallRunDirection * WallRunDirectionRatio + m_WallNormal * WallRunNormalRatio + FVector::UpVector * WallRunUpRatio).GetSafeNormal();
	MotionController->CancelRootMotion(m_WallRunMainMotionId);
    
	FRootMotionRequest WallJumpRequest;
	WallJumpRequest.MovementName = FName("WallJump");
	WallJumpRequest.Direction = JumpDirection;
	WallJumpRequest.Strength = WallJumpForce;
	WallJumpRequest.Duration = 0.1;
	WallJumpRequest.bIsAdditive = false; 
	WallJumpRequest.Priority = RootMotionPriority::High;
	WallJumpRequest.VelocityOnFinish = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
	WallJumpRequest.bEnableGravity = false;
    
	m_WallRunMainMotionId = MotionController->ApplyRootMotion(WallJumpRequest);
}

FName USR_WallJumpComponent::GetStateName() const
{
	return FName("WallJump");
}

int32 USR_WallJumpComponent::GetStatePriority() const
{
	return 0;
}

bool USR_WallJumpComponent::IsStateActive() const
{
	return bIsActive;
}

void USR_WallJumpComponent::OnJumpButtonPressed()
{
}

void USR_WallJumpComponent::OnJumpButtonReleased()
{
}

