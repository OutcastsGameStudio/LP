// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_WallRunComponent.h"

#include "Components/CapsuleComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/WallJump/SR_WallJumpComponent.h"


// Sets default values for this component's properties
USR_WallRunComponent::USR_WallRunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USR_WallRunComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();

	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_WallRunComponent::BeginPlay()"));
		return;
	}

	OwnerCharacter->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &USR_WallRunComponent::OnHit);
	OwnerCharacter->OnMoveForwardInputPressed.AddDynamic(this, &USR_WallRunComponent::OnMoveForwardInputPressed);
	OwnerCharacter->OnMoveForwardInputReleased.AddDynamic(this, &USR_WallRunComponent::OnMoveForwardInputReleased);

	OwnerCharacter->FOnJumpInputPressed.AddDynamic(this, &USR_WallRunComponent::OnJumpButtonPressed);

	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_WallRunComponent::LeaveState);
}


// Called every frame
void USR_WallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
	// Vérifier si le wall run doit continuer
	if (bIsStateActive)
	{
		// Vérifier si on est toujours contre un mur
		FHitResult Hit;
		if (!DetectNextWall(Hit))
		{
			// Plus de mur, arrêter le wall run
			// StopWallRun();
		}
		else
		{
			// Vérifier l'angle
			auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
			auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
			auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
			if (angle > MaxAngleBeforeStop)
			{
				// StopWallRun();
			}
		}
	}
    
	// Garder le check pour savoir si on peut wall run
	// CanWallRun();

	// ...
}

bool USR_WallRunComponent::DetectNextWall(FHitResult& Hit)
{
	UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent();
	FName CollisionProfile = Capsule->GetCollisionProfileName();
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start - m_WallNormal * Capsule->GetScaledCapsuleRadius();
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(OwnerCharacter);
	GetWorld()->SweepSingleByProfile(Hit, Start, End, Capsule->GetComponentQuat(), CollisionProfile, Capsule->GetCollisionShape(), Params);
	FVector Test = Start - m_WallNormal * Capsule->GetScaledCapsuleRadius();
	Test = Capsule->GetComponentQuat().RotateVector(Test);
	DrawDebugLine(GetWorld(), Start, Test, FColor::Green, false, 0.1f, 0, 1.f);
	return Hit.bBlockingHit;
}

void USR_WallRunComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bIsStateActive && m_IsMovingForward && FMath::Abs(Hit.Normal.Z) < MAX_Z_THRE_HOLD &&  CheckIfNotInCorner())
	{
		auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
		auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
		auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
		if(angle <= 15 )
		{
			bIsStateActive =  false;
			return;
		};
		m_WallNormal = Hit.Normal;
		FVector WallDirection = FVector::CrossProduct(FVector::UpVector, Hit.Normal);
		m_WallRunDirection = (WallDirection * (OwnerCharacter->GetActorForwardVector().Dot(WallDirection) > 0.f ? 1.f : -1.f)).GetSafeNormal();

		// enter the wall run state
		USR_ContextStateComponent* ContextState = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
		ContextState->TransitionState(MotionState::WALL_RUN);
		bIsStateActive =  true;
	}
}

void USR_WallRunComponent::EnterState(void* data)
{
	if (bIsStateActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("USR_WallRunComponent::EnterState() - State is already active"));
		return;
	}
	UpdateState();
	bIsStateActive = true;
}

void USR_WallRunComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if(rootMotionId != m_WallRunMainMotionId) return;
	bIsStateActive = false;
	StopWallRun();
	ContextStateComponent->TransitionState(MotionState::NONE, bForced);
}

bool USR_WallRunComponent::LookAheadQuery()
{
	return bIsStateActive;
}

void USR_WallRunComponent::UpdateState()
{
	// Annuler tout mouvement existant
	if (m_WallRunMainMotionId != -1)
	{
		MotionController->CancelRootMotion(m_WallRunMainMotionId);
		m_WallRunMainMotionId = -1;
	}

	DrawDebugLine(
		GetWorld(),
		OwnerCharacter->GetActorLocation(),
		OwnerCharacter->GetActorLocation() + m_WallRunDirection,
		FColor::Green,
		false,
		10.f,
		0,
		1.f
	);

	DrawDebugLine(
	GetWorld(),
	OwnerCharacter->GetActorLocation(),
	OwnerCharacter->GetActorLocation() + m_WallRunDirection + FVector(0, 0, 1),
	FColor::Red,
	false,
	10.f,
	0,
	1.f
);
	
	// Configurer la requête pour le mouvement horizontal
	FRootMotionRequest WallRunRequest;
	WallRunRequest.MovementName = FName("WallRun_Main");
	WallRunRequest.Direction = m_WallRunDirection;
	WallRunRequest.Strength = 1.0f * ForceMultiplier;
	WallRunRequest.Duration = Duration; // @TODO Configurable
	WallRunRequest.bIsAdditive = true;
	WallRunRequest.Priority = RootMotionPriority::Medium;
	WallRunRequest.StrengthOverTime = WallRunStrengthCurve;
	WallRunRequest.VelocityOnFinish = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
	// WallRunRequest.SetVelocityOnFinish = m_WallRunDirection * MaxWalkSpeed * 0.8f;
	// WallRunRequest.bEnableGravity = true;
    
	// Appliquer le mouvement principal
	m_WallRunMainMotionId = MotionController->ApplyRootMotion(WallRunRequest);
    
	// Configurer la requête pour coller au mur
	FRootMotionRequest WallStickRequest;
	WallStickRequest.MovementName = FName("WallRun_Stick");
	WallStickRequest.Direction = -m_WallNormal;
	WallStickRequest.Strength = 1.0f * StickingRatio;
	WallStickRequest.Duration = Duration;
	WallStickRequest.bIsAdditive = true;
	WallStickRequest.Priority = RootMotionPriority::Low;
	WallStickRequest.VelocityOnFinish = ERootMotionFinishVelocityMode::ClampVelocity;
	WallStickRequest.bEnableGravity = false;
 //    
	// // Appliquer la force de collage au mur
	MotionController->ApplyRootMotion(WallStickRequest);
 //    
	// // Appliquer une force vers le bas progressive mais plus faible
	// FRootMotionRequest GravityRequest;
	// GravityRequest.MovementName = FName("WallRun_Gravity");
	// GravityRequest.Direction = FVector(0, 0, -1);
	// GravityRequest.Strength = WallRunFallingAcceleration;
	// GravityRequest.Duration = 3.0f;
	// GravityRequest.bIsAdditive = true;
	// GravityRequest.Priority = RootMotionPriority::Low;
	// GravityRequest.bEnableGravity = false;
 //    
	// MotionController->ApplyRootMotion(GravityRequest);
}

FName USR_WallRunComponent::GetStateName() const
{
	return FName("WallRun");
}

int32 USR_WallRunComponent::GetStatePriority() const
{
	return 1; //@TODO: see what priority we want to give to the wall run and what the async root movement do with this 
}

bool USR_WallRunComponent::IsStateActive() const
{
	return bIsStateActive;
}

void USR_WallRunComponent::OnMoveForwardInputPressed()
{
	m_IsMovingForward = true;
}

void USR_WallRunComponent::OnMoveForwardInputReleased()
{
	m_IsMovingForward = false;
}


// check if the character i not going to trigger a wall run on a corner
bool USR_WallRunComponent::CheckIfNotInCorner()
{
	float rotation = 45.f;
	FRotator Rotator1(0.f, rotation, 0.f);
	FRotator Rotator2(0.f, -rotation, 0.f);

	FQuat Quat1(Rotator1);
	FQuat Quat2(Rotator2);
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());

	if(PC == nullptr) return false;
	FRotator CameraRotation;
	FVector CameraLocation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	
	FVector CameraDirection = CameraRotation.Vector();  // This gives us the direction the camera is facing
	float magnitude = 300.f;


	FVector LeftVector = Quat1.RotateVector(CameraDirection);
	FVector RightVector = Quat2.RotateVector(CameraDirection);


	// leftVector rotated to angle
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);  // Ignore the character itself
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		OwnerCharacter->GetActorLocation(),
		OwnerCharacter->GetActorLocation() + RightVector * magnitude,
		ECC_Visibility,
		QueryParams
	);

	//rightVector rotated to angle
	FHitResult HitResult1;
	FCollisionQueryParams QueryParams1;
	QueryParams.AddIgnoredActor(OwnerCharacter);  // Ignore the character itself
	bool bHit1 = GetWorld()->LineTraceSingleByChannel(
		HitResult1,
		OwnerCharacter->GetActorLocation(),
		OwnerCharacter->GetActorLocation() + LeftVector * magnitude,
		ECC_Visibility,  // Or use another channel like ECC_Pawn
		QueryParams1
	);

	// DebugLineTrace(HitResult, bHit, FColor::Green,OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + RightVector * magnitude);
	// DebugLineTrace(HitResult1, bHit1, FColor::Yellow, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + LeftVector * magnitude);
	auto u = !(bHit && bHit1);
	return !(bHit && bHit1);
}

void USR_WallRunComponent::StopWallRun()
{
	bIsStateActive = false;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	ContextStateComponent->TransitionState(MotionState::NONE, true);
	// OwnerCharacter->GetCharacterMovement()->SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
}

void USR_WallRunComponent::OnJumpButtonPressed()
{
	if(!bIsStateActive) return;
	bIsStateActive = false;
	FWallJumpData WallJumpData;
	WallJumpData.WallRunDirection = m_WallRunDirection;
	WallJumpData.WallNormal = m_WallNormal;

	void* Data = &WallJumpData;
	ContextStateComponent->TransitionState(MotionState::WALL_JUMP, Data, true);
}

void USR_WallRunComponent::OnJumpButtonReleased()
{
}

void USR_WallRunComponent::DebugLineTrace(FHitResult hitResult, bool hit,FColor color, FVector vectorStart, FVector vectorEnd )
{
	DrawDebugLine(
		GetWorld(),
		vectorStart,
		 vectorEnd,
		color,
		false,
		0.1f,
		0,
		1.f
	);
	if(hit)
	{
		// Draw a point at the impact point
		DrawDebugPoint(
			GetWorld(),
			hitResult.ImpactPoint,
			10.0f,                  // Size of the point
			color,          // Color on hit
			false,                  // Persistent
			5.0f                    // Duration
		);
	}
}

