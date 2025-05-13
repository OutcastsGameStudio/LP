// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_WallRunComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/WallJump/SR_WallJumpComponent.h"


// Sets default values for this component's properties
USR_WallRunComponent::USR_WallRunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void USR_WallRunComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();
	PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());

	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent || !PlayerController)
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


void USR_WallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ResetCameraRotation(DeltaTime);

	auto velocity = FVector(CharacterMovement->Velocity.X, CharacterMovement->Velocity.Y, 0).Size();
	if (bIsStateActive)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("TICK"));
		if(CharacterMovement->IsMovingOnGround())
		{
			LeaveState(-1, true);
			return;
		}
		FHitResult Hit;
		// if no wall is detected, stop the wall run
		if (!DetectNextWall(Hit))
		{
			LeaveState(-1, true);
		}
		else
		{
			auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
			auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
			auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
			if (angle > MaxAngleBeforeStop)
			{
				LeaveState(-1, true);
			}
		}
	}
    
	if(!bIsStateActive) return;
	UpdateState(DeltaTime);
}

bool USR_WallRunComponent::HasDetectedPlayerMouseRotation(float DeltaTime)
{
	float CurrentMousePosX, CurrentMousePosY;
	PlayerController->GetMousePosition(CurrentMousePosX, CurrentMousePosY);
    
	const float MovementThreshold = 0.1f;

	if (FMath::Abs(CurrentMousePosX - m_MousePosXAtWallRunStart) > MovementThreshold ||
		FMath::Abs(CurrentMousePosY - m_MousePosYAtWallRunStart) > MovementThreshold)
	{
		return true;
	}
    
	return false;
}

void USR_WallRunComponent::UpdateCameraRotation(float DeltaTime)
{
    bool hasDetectedPlayerMouseRotation = HasDetectedPlayerMouseRotation(DeltaTime);
    if(hasDetectedPlayerMouseRotation)
        return;
    
    float TargetRoll = m_WallRunSide * m_WallRunCameraRollAngle;
    m_CurrentCameraRoll = FMath::FInterpTo(m_CurrentCameraRoll, -TargetRoll, DeltaTime, m_CameraRollSpeed);
    
    FRotator CurrentRotation = PlayerController->GetControlRotation();
    float CurrentYaw = CurrentRotation.Yaw;
    float TargetYaw = m_WallRunDirection.Rotation().Yaw;

	// for a right side wall (m_WallRunSide = 1), the offset is negative (camera outwards)
	// for a left side wall (m_WallRunSide = -1), the offset is positive (camera inwards)
	float YawOffset = -m_WallRunSide * m_WallRunCameraOffset;

    // Use FindDeltaAngleDegrees to get the difference between the current and target yaw without bothering about the wrap-around where 180 degrees becomes -180 degrees
    float YawDelta = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw) + YawOffset;
    
    float NewTargetYaw = CurrentYaw + YawDelta;
    
    float NewYaw = FMath::FInterpTo(CurrentYaw, NewTargetYaw, DeltaTime, m_CameraRollSpeed);
    
    PlayerController->SetControlRotation(FRotator(CurrentRotation.Pitch, NewYaw, m_CurrentCameraRoll));
}

void USR_WallRunComponent::ResetCameraRotation(float DeltaTime)
{
	if (!bResettingCamera)
		return;
    
	m_CameraResetTimer += DeltaTime;
    
	float Alpha = FMath::Clamp(m_CameraResetTimer / m_CameraResetDuration, 0.0f, 1.0f);
    
	m_CurrentCameraRoll = FMath::Lerp(m_StartCameraRoll, 0.0f, Alpha);
    
	FRotator CurrentRotation = PlayerController->GetControlRotation();
	PlayerController->SetControlRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, m_CurrentCameraRoll));
    
	if (m_CameraResetTimer >= m_CameraResetDuration)
	{
		bResettingCamera = false;
		m_CurrentCameraRoll = 0.0f;
	}
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
	return Hit.bBlockingHit;
}

void USR_WallRunComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bIsStateActive
		&& m_IsMovingForward
		&& FMath::Abs(Hit.Normal.Z) < MAX_Z_THRE_HOLD
		&&  CheckIfNotInCorner()
		&& CharacterMovement->MovementMode == MOVE_Falling
		&& FVector(CharacterMovement->Velocity.X, CharacterMovement->Velocity.Y, 0).Size() > MinWallRunSpeed
		)
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
		m_WallRunSide = FVector::DotProduct(WallDirection, OwnerCharacter->GetActorForwardVector()) > 0.f ? 1 : -1;
		m_WallRunDirection = (WallDirection * m_WallRunSide).GetSafeNormal();

		// enter the wall run state
		USR_ContextStateComponent* ContextState = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
		ContextState->TransitionState(MotionState::WALL_RUN);
		
		// retrieve mouse position
		PlayerController->GetMousePosition(m_MousePosXAtWallRunStart, m_MousePosYAtWallRunStart);
		
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
	bIsStateActive = true;
	CharacterMovement->SetMovementMode(MOVE_Custom);
}

void USR_WallRunComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if(!bForced && rootMotionId != m_rootMotionId) return;
	StopWallRun();
	ContextStateComponent->TransitionState(MotionState::NONE, bForced);
}

bool USR_WallRunComponent::LookAheadQuery()
{
	return bIsStateActive;
}

void USR_WallRunComponent::UpdateState()
{
}

FName USR_WallRunComponent::GetStateName() const
{
	return FName("WallRun");
}

int32 USR_WallRunComponent::GetStatePriority() const
{
	return 1; //@TODO: see what priority we want to give to the wall run and what the async root movement do with this, should be used in asyncmovement request payload but OSEF for now
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

// check if the character is not going to trigger a wall run on a corner
bool USR_WallRunComponent::CheckIfNotInCorner()
{
	float rotation = 45.f;
	FRotator Rotator1(0.f, rotation, 0.f);
	FRotator Rotator2(0.f, -rotation, 0.f);

	FQuat Quat1(Rotator1);
	FQuat Quat2(Rotator2);

	FRotator CameraRotation;
	FVector CameraLocation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	
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

	/*
	 * Debugging
	 */
	// DebugLineTrace(HitResult, bHit, FColor::Green,OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + RightVector * magnitude);
	// DebugLineTrace(HitResult1, bHit1, FColor::Yellow, OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + LeftVector * magnitude);
	/*
	* Debugging
	*/
	
	return !(bHit && bHit1);
}

void USR_WallRunComponent::StopWallRun()
{
	bIsStateActive = false;
	WallRunFallingSpeed = 0;
	m_WallRunSide = 0;

	m_StartCameraRoll = m_CurrentCameraRoll;
	m_CameraResetTimer = 0.0f;
	bResettingCamera = true;
	
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	ContextStateComponent->TransitionState(MotionState::NONE, true);
}

void USR_WallRunComponent::UpdateState(float deltaTime)
{
    WallRunFallingSpeed += WallRunFallingAcceleration * deltaTime;
    FVector CurrentVelocity = CharacterMovement->Velocity;
    float CurrentSpeed = FVector::DotProduct(CurrentVelocity, m_WallRunDirection);

	float WallRunSpeed = FMath::Clamp(CurrentSpeed, MinWallRunClampedSpeed, MaxWallRunClampedSpeed);
    CharacterMovement->Velocity = m_WallRunDirection * WallRunSpeed;
 
    // controlled gravity
    CharacterMovement->Velocity.Z = CharacterMovement->Velocity.Z - WallRunFallingSpeed;
    
    FVector Delta = CharacterMovement->Velocity * deltaTime;
    FHitResult Hit(1.f);

    CharacterMovement->SafeMoveUpdatedComponent(Delta, CharacterMovement->UpdatedComponent->GetComponentRotation(), true, Hit);

	UpdateCameraRotation(deltaTime);

    if (Hit.IsValidBlockingHit() && Hit.Normal.Z > 0.f)
    {
        if (Hit.Normal.Z > 0.5f)
        {
            CharacterMovement->SetMovementMode(MOVE_Walking, 0);
        }
    }
    else if (!DetectNextWall(Hit))
    {
    	LeaveState(-1, true);
        // bIsStateActive = false;
        // CharacterMovement->SetMovementMode(MOVE_Falling);
        // CharacterMovement->SafeMoveUpdatedComponent(Delta, CharacterMovement->UpdatedComponent->GetComponentRotation(), true, Hit); // ?? what is it for ??
    }
    
    auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
    auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
    auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
    if(angle > MaxAngleBeforeStop)
    {
    	LeaveState(-1, true);
    }
}

void USR_WallRunComponent::OnJumpButtonPressed()
{
	if(!bIsStateActive) return;
	LeaveState(-1); 
	FWallJumpData WallJumpData;
	WallJumpData.WallRunDirection = m_WallRunDirection;
	WallJumpData.WallNormal = m_WallNormal;
	void* Data = &WallJumpData;
	ContextStateComponent->TransitionState(MotionState::WALL_JUMP, Data, true);
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
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

