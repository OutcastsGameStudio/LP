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
        
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("WallRunDirection: ") + m_WallRunDirection.ToString());
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast data to FWallJumpData"));
	}
    
	// Appliquer le wall jump
	UpdateState();
}

void USR_WallJumpComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if(rootMotionId != m_WallRunMainMotionId) return;
	ContextStateComponent->TransitionState(MotionState::NONE, bForced);
}

bool USR_WallJumpComponent::LookAheadQuery()
{
	return false;
}

void USR_WallJumpComponent::UpdateState()
{
	// print normal
	FVector JumpDirection = (m_WallRunDirection * 2 + m_WallNormal + FVector::UpVector * 2).GetSafeNormal();
    
	// Annuler les mouvements de wall run existants
	
	MotionController->CancelRootMotion(m_WallRunMainMotionId);
	// m_WallRunMainMotionId = -1;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("WallNormal: ") + JumpDirection.ToString());
    
	// Configurer la requête pour le saut depuis le mur
	FRootMotionRequest WallJumpRequest;
	WallJumpRequest.MovementName = FName("WallJump");
	WallJumpRequest.Direction = JumpDirection;
	WallJumpRequest.Strength = WallJumpForce; // Utiliser la même variable que dans votre code original
	WallJumpRequest.Duration = 0.1; // Court, juste pour l'impulsion
	WallJumpRequest.bIsAdditive = false; // Remplacer les autres mouvements
	WallJumpRequest.Priority = RootMotionPriority::High;
	WallJumpRequest.VelocityOnFinish = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
	// WallJumpRequest.SetVelocityOnFinish = JumpDirection * WallJumpSpeed * 0.8f; // Conserver une partie de l'élan
	WallJumpRequest.bEnableGravity = true;
    
	// Appliquer le mouvement de saut
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
	return false;
}

void USR_WallJumpComponent::OnJumpButtonPressed()
{
}

void USR_WallJumpComponent::OnJumpButtonReleased()
{
}

