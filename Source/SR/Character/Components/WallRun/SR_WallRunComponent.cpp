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
			// StopWallRun(); @TODO: maybe need it ?
		}
		else
		{
			// Vérifier l'angle
			auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
			auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
			auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
			if (angle > MaxAngleBeforeStop)
			{
				// StopWallRun(); @TODO: maybe need it ?
			}
		}
	}
    
	if(!bIsStateActive) return;
	UpdateState(DeltaTime);
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
	if(!bIsStateActive && m_IsMovingForward && FMath::Abs(Hit.Normal.Z) < MAX_Z_THRE_HOLD &&  CheckIfNotInCorner() && CharacterMovement->MovementMode == MOVE_Falling)
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
	bIsStateActive = true;
	CharacterMovement->SetMovementMode(MOVE_Custom);
}

void USR_WallRunComponent::LeaveState(int32 rootMotionId, bool bForced)
{
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
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	ContextStateComponent->TransitionState(MotionState::NONE, true);
}

void USR_WallRunComponent::UpdateState(float deltaTime)
{
    WallRunFallingSpeed += WallRunFallingAcceleration * deltaTime;
    
    FVector CurrentVelocity = CharacterMovement->Velocity;
    float CurrentSpeed = FVector::DotProduct(CurrentVelocity, m_WallRunDirection);
    float WallRunSpeed = CurrentSpeed;
    
    CharacterMovement->Velocity = m_WallRunDirection * WallRunSpeed;
 
	// controlled gravity
    CharacterMovement->Velocity.Z = CharacterMovement->Velocity.Z - WallRunFallingSpeed;
    
    FVector Delta = CharacterMovement->Velocity * deltaTime;
    FHitResult Hit(1.f);

    CharacterMovement->SafeMoveUpdatedComponent(Delta, CharacterMovement->UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit() && Hit.Normal.Z > 0.f)
    {
        if (Hit.Normal.Z > 0.5f)
        {
            CharacterMovement->SetMovementMode(MOVE_Walking, 0);
        }
    }
    else if (!DetectNextWall(Hit))
    {
        bIsStateActive = false;
        CharacterMovement->SetMovementMode(MOVE_Falling);
        CharacterMovement->SafeMoveUpdatedComponent(Delta, CharacterMovement->UpdatedComponent->GetComponentRotation(), true, Hit);
    }
    
    auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
    auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
    auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
    if(angle > MaxAngleBeforeStop)
    {
        bIsStateActive = false;
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }
}

void USR_WallRunComponent::OnJumpButtonPressed()
{
	if(!bIsStateActive) return;
	WallRunFallingSpeed = 0;
	bIsStateActive = false;
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

