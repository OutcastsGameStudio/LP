// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_SlideComponent.h"

#include "MaterialHLSLTree.h"
#include "SR/Character/SR_Character.h"

// Sets default values for this component's properties
USR_SlideComponent::USR_SlideComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerCharacter = nullptr;
	MotionController = nullptr;
	ContextStateComponent = nullptr;
	CapsuleComponent = nullptr;
	MeshComponent = nullptr;
	CharacterMovement = nullptr;
}

// Called when the game starts
void USR_SlideComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();
	CapsuleComponent = GetOwner()->FindComponentByClass<UCapsuleComponent>();
	MeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();

	Friction = CharacterMovement->BrakingFriction;
	Gravity = 9.80f;
	InitialCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();

	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_DashComponent::BeginPlay()"));
		return;
	}

	OwnerCharacter->FOnSlideInputPressed.AddDynamic(this, &USR_SlideComponent::Slide);
	OwnerCharacter->FOnSlideInputReleased.AddDynamic(this, &USR_SlideComponent::StopSlide);

	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_SlideComponent::LeaveState);
}

// Called every frame
void USR_SlideComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsStateActive)
	{
		ProcessSlide(DeltaTime);
	}
}

void USR_SlideComponent::StartSlide()
{
	InitializeSlideState();
	UpdateSlideDirection();
	AdjustCharacterCollision();

	if (StartedSlideAnimMontage)
	{
		OwnerCharacter->PlayAnimMontage(StartedSlideAnimMontage);
	}
}

bool USR_SlideComponent::CanInitiateSlide() const
{
	if (CharacterMovement->GetLastUpdateVelocity().IsNearlyZero() || CharacterMovement->IsFalling() || bIsCrouching)
	{
		return false;
	}

	return true;
}

void USR_SlideComponent::InitializeSlideState()
{
	bIsSliding = true;
	OwnerCharacter->SetMoveInputBlocked(true);
	SlideStartLocation = GetOwner()->GetActorLocation();
	SlideDirection = GetOwner()->GetActorForwardVector();
	SlideSpeed = 950.0f;
	CurrentSlideDistance = 0.0f;
}

FVector USR_SlideComponent::UpdateSlideDirection()
{
	FHitResult HitResult;

	if (PerformGroundCheck(HitResult))
	{
		const FVector FPlayerForward = GetOwner()->GetActorForwardVector();
		SlideDirection = FVector::VectorPlaneProject(FPlayerForward, HitResult.Normal).GetSafeNormal();
	}

	return SlideDirection;
}

bool USR_SlideComponent::PerformGroundCheck(FHitResult &OutHitResult) const
{
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 100.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility, QueryParams);
}

void USR_SlideComponent::AdjustCharacterCollision()
{
	if (CapsuleComponent && MeshComponent)
	{
		CapsuleComponent->SetCapsuleHalfHeight(CapsuleHalfHeightSliding);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT("Failed to load CapsuleComponent or MeshComponent in "
			       "USR_SlideComponent::AdjustCharacterCollision()"));
	}
}

void USR_SlideComponent::ProcessSlide(float DeltaTime)
{
	SlideSpeed = CalculateSlideSpeed(DeltaTime);

	FVector NewLocation = GetOwner()->GetActorLocation() + (SlideDirection * SlideSpeed * DeltaTime);

	if (CheckCollisionAtNewPosition(NewLocation))
	{
		StopSlide();
	}
	else
	{
		GetOwner()->SetActorLocation(NewLocation);
	}
}

bool USR_SlideComponent::CheckCollisionAtNewPosition(const FVector &NewLocation) const
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(HitResult, GetOwner()->GetActorLocation(), NewLocation, ECC_Visibility,
	                                            CollisionParams);
}

void USR_SlideComponent::StopSlide()
{
	UE_LOG(LogTemp, Warning, TEXT("StopSlide() called - bIsSliding: %s, bIsStateActive: %s"),
	       bIsSliding ? TEXT("true") : TEXT("false"),
	       bIsStateActive ? TEXT("true") : TEXT("false"));

	bIsSliding = false;
	bIsStateActive = false;
	CurrentSlideDistance = 0.0f;
	SlideSpeed = 0.0f;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + FVector(0, 0, InitialCapsuleHalfHeight * 2.0f);

	FHitResult HitResult;
	bool bHitCeiling = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	if (bHitCeiling)
	{
		bIsCrouching = true;
		CharacterMovement->bWantsToCrouch = true;
		CharacterMovement->Crouch();
		UE_LOG(LogTemp, Warning, TEXT("StopSlide() - Forced crouch due to ceiling"));
	}
	else
	{
		bIsCrouching = false;
		CharacterMovement->bWantsToCrouch = false;
		CharacterMovement->UnCrouch();
		if (CapsuleComponent && MeshComponent)
		{
			CapsuleComponent->SetCapsuleHalfHeight(InitialCapsuleHalfHeight);
			MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		}
		UE_LOG(LogTemp, Warning, TEXT("StopSlide() - Restored normal dimensions"));
	}

	if (CharacterMovement)
	{
		CharacterMovement->SetMovementMode(MOVE_Walking);
		UE_LOG(LogTemp, Warning, TEXT("StopSlide() - Reset movement mode to Walking"));
	}

	LeaveState(CurrentRootMotionID, true);
}

float USR_SlideComponent::GetCurrentFloorAngle()
{
	if (!OwnerCharacter)
	{
		return 0.0f;
	}

	FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
	ForwardVector.Z = 0.0f;
	ForwardVector.Normalize();

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start - FVector(0, 0, 100);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	if (OwnerCharacter->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		FVector SurfaceNormal = HitResult.Normal;

		FVector ProjectedNormal = SurfaceNormal - (ForwardVector * FVector::DotProduct(SurfaceNormal, ForwardVector));
		ProjectedNormal.Normalize();

		float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(SurfaceNormal, FVector(0, 0, 1))));

		float DirectionModifier = FVector::DotProduct(ForwardVector, SurfaceNormal) > 0 ? -1.0f : 1.0f;

		return SlopeAngle * DirectionModifier;
	}

	return 0.0f;
}

float USR_SlideComponent::CalculateSlideSpeed(float DeltaTime)
{
	float CurrentFloorAngle = GetCurrentFloorAngle();

	if (FMath::IsNearlyZero(CurrentFloorAngle, 1.0f))
	{
		return ProcessBasicSlide(DeltaTime);
	}

	float GravityForce = Gravity * sin(CurrentFloorAngle);
	float resultGravityForce = GravityForce - Friction * Gravity * cos(CurrentFloorAngle);

	/*
   * If the player is sliding down a slope, we want to increase the slide speed.
   * If the player is sliding up a slope, we want to decrease the slide speed by
   */
	float AngleMultiplier = (CurrentFloorAngle > 0.0f) ? Friction : 1.0f;

	float NewSlideSpeed = SlideSpeed + resultGravityForce * DeltaTime * AngleMultiplier;

	return NewSlideSpeed;
}

float USR_SlideComponent::ProcessBasicSlide(float DeltaTime)
{
	float NewSlideSpeed = FMath::Max(0.0f, SlideSpeed - BasicSlideDeceleration * DeltaTime);

	CurrentSlideDistance += SlideSpeed * DeltaTime;

	if (CurrentSlideDistance >= MaxSlideDistance || FMath::IsNearlyZero(NewSlideSpeed, 10.0f))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { StopSlide(); });
	}

	return NewSlideSpeed;
}

void USR_SlideComponent::Slide()
{
	ContextStateComponent->TransitionState(MotionState::SLIDE);

	if (!CanInitiateSlide())
	{
		return;
	}

	StartSlide();
}

void USR_SlideComponent::UpdateState() { bIsStateActive = true; }

void USR_SlideComponent::EnterState(void *data)
{
	if (bIsStateActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("USR_SlideComponent::EnterState() - State is already active"));
		return;
	}

	UpdateState();
}

void USR_SlideComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if (!bForced && rootMotionId != CurrentRootMotionID)
	{
		return;
	}

	if (!ContextStateComponent)
		UE_LOG(LogTemp, Error, TEXT("Failed to load ContextState in USR_SlideComponent::LeaveState()"));

	if (OwnerCharacter)
	{
		OwnerCharacter->SetMoveInputBlocked(false);
	}

	if (StoppedSlideAnimMontage)
	{
		OwnerCharacter->PlayAnimMontage(StoppedSlideAnimMontage);
	}

	bIsStateActive = false;
	ContextStateComponent->TransitionState(MotionState::NONE, true);
}

bool USR_SlideComponent::LookAheadQuery() { return !bIsStateActive; }

FName USR_SlideComponent::GetStateName() const { return FName("Slide"); }

int32 USR_SlideComponent::GetStatePriority() const { return 0; }

bool USR_SlideComponent::IsStateActive() const { return bIsStateActive; }
