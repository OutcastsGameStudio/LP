// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_WallRunComponent.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "SR/Character/Components/WallJump/SR_WallJumpComponent.h"
#include "SR/Character/SR_Character.h"

// Sets default values for this component's properties
USR_WallRunComponent::USR_WallRunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked
	// every frame.  You can turn these features off to improve performance if you
	// don't need them.
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
}

void USR_WallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										 FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ResetCameraRotation(DeltaTime);
	if (bIsStateActive)
	{
		if (CharacterMovement->IsMovingOnGround())
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

	if (!bIsStateActive)
	{
		return;
	}
	UpdateState(DeltaTime);
}

bool USR_WallRunComponent::HasDetectedPlayerMouseRotation(float DeltaTime)
{
	float CurrentMousePosX, CurrentMousePosY;
	PlayerController->GetMousePosition(CurrentMousePosX, CurrentMousePosY);

	const float MovementThreshold = 0.1f;

	if (FMath::Abs(CurrentMousePosX - MousePosXAtWallRunStart) > MovementThreshold ||
		FMath::Abs(CurrentMousePosY - MousePosYAtWallRunStart) > MovementThreshold)
	{
		return true;
	}

	return false;
}

void USR_WallRunComponent::UpdateCameraRotation(float DeltaTime)
{
	bool hasDetectedPlayerMouseRotation = HasDetectedPlayerMouseRotation(DeltaTime);
	if (hasDetectedPlayerMouseRotation)
	{
		return;
	}

	float TargetRoll = WallRunSide * WallRunCameraRollAngle;
	CurrentCameraRoll = FMath::FInterpTo(CurrentCameraRoll, -TargetRoll, DeltaTime, CameraRollSpeed);

	FRotator CurrentRotation = PlayerController->GetControlRotation();
	float CurrentYaw = CurrentRotation.Yaw;
	float TargetYaw = WallRunDirection.Rotation().Yaw;

	// for a right side wall (WallRunSide = 1), the offset is negative (camera
	// outwards) for a left side wall (WallRunSide = -1), the offset is positive
	// (camera inwards)
	float YawOffset = -WallRunSide * WallRunCameraOffset;

	// Use FindDeltaAngleDegrees to get the difference between the current and
	// target yaw without bothering about the wrap-around where 180 degrees
	// becomes -180 degrees
	float YawDelta = FMath::FindDeltaAngleDegrees(CurrentYaw, TargetYaw) + YawOffset;

	float NewTargetYaw = CurrentYaw + YawDelta;

	float NewYaw = FMath::FInterpTo(CurrentYaw, NewTargetYaw, DeltaTime, CameraRollSpeed);

	PlayerController->SetControlRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentCameraRoll));
}

void USR_WallRunComponent::ResetCameraRotation(float DeltaTime)
{
	if (!bResettingCamera)
	{
		return;
	}

	CameraResetTimer += DeltaTime;

	float Alpha = FMath::Clamp(CameraResetTimer / CameraResetDuration, 0.0f, 1.0f);

	CurrentCameraRoll = FMath::Lerp(StartCameraRoll, 0.0f, Alpha);

	FRotator CurrentRotation = PlayerController->GetControlRotation();
	PlayerController->SetControlRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentCameraRoll));

	if (CameraResetTimer >= CameraResetDuration)
	{
		bResettingCamera = false;
		CurrentCameraRoll = 0.0f;
	}
}

bool USR_WallRunComponent::DetectNextWall(FHitResult &Hit)
{
	UCapsuleComponent *Capsule = OwnerCharacter->GetCapsuleComponent();
	FName CollisionProfile = Capsule->GetCollisionProfileName();
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start - WallNormal * Capsule->GetScaledCapsuleRadius();
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(OwnerCharacter);
	GetWorld()->SweepSingleByProfile(Hit, Start, End, Capsule->GetComponentQuat(), CollisionProfile,
									 Capsule->GetCollisionShape(), Params);
	return Hit.bBlockingHit;
}

void USR_WallRunComponent::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
								 FVector NormalImpulse, const FHitResult &Hit)
{
	if (!bIsStateActive && bIsMovingForward && FMath::Abs(Hit.Normal.Z) < MaxZThreshold && CheckIfNotInCorner() &&
		CharacterMovement->MovementMode == MOVE_Falling &&
		OtherActor->ActorHasTag("WALL_RUN") &&
		FVector(CharacterMovement->Velocity.X, CharacterMovement->Velocity.Y, 0).Size() > MinWallRunSpeed)
	{
		auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
		auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
		auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
		if (angle <= 15)
		{
			bIsStateActive = false;
			return;
		};
		WallNormal = Hit.Normal;
		FVector WallDirection = FVector::CrossProduct(FVector::UpVector, Hit.Normal);
		WallRunSide = FVector::DotProduct(WallDirection, OwnerCharacter->GetActorForwardVector()) > 0.f
			? 1
			: -1; // 1 => right wall, -1 => left wall
		bIsWallRunningLeft = WallRunSide < 0;
		WallRunDirection = (WallDirection * WallRunSide).GetSafeNormal();

		LockedRotation = UKismetMathLibrary::FindLookAtRotation(
			OwnerCharacter->GetActorLocation(), 
			OwnerCharacter->GetActorLocation() + WallRunDirection
		);

		// enter the wall run state
		USR_ContextStateComponent *ContextState = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
		ContextState->TransitionState(MotionState::WALL_RUN);

		// retrieve mouse position
		PlayerController->GetMousePosition(MousePosXAtWallRunStart, MousePosYAtWallRunStart);

		bIsStateActive = true;
	}
}

void USR_WallRunComponent::EnterState(void *data)
{
	if (bIsStateActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("USR_WallRunComponent::EnterState() - State is already active"));
		return;
	}
	bIsStateActive = true;

	OnWallRunStarted.Broadcast();

	CharacterMovement->SetMovementMode(MOVE_Custom);
}

void USR_WallRunComponent::LeaveState(int32 rootMotionId, bool bForced)
{
	if (!bForced && rootMotionId != RootMotionId)
	{
		return;
	}
	OnWallRunEnded.Broadcast();
	StopWallRun();

	ContextStateComponent->TransitionState(MotionState::NONE, bForced);
}

bool USR_WallRunComponent::LookAheadQuery() { return bIsStateActive; }

void USR_WallRunComponent::UpdateState() {}

FName USR_WallRunComponent::GetStateName() const { return FName("WallRun"); }

int32 USR_WallRunComponent::GetStatePriority() const
{
	return 1; //@TODO: see what priority we want to give to the wall run and what
		// the async root movement do with this, should be used in
		// asyncmovement request payload but OSEF for now
}

bool USR_WallRunComponent::IsStateActive() const { return bIsStateActive; }

void USR_WallRunComponent::OnMoveForwardInputPressed() { bIsMovingForward = true; }

void USR_WallRunComponent::OnMoveForwardInputReleased() { bIsMovingForward = false; }

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

	FVector CameraDirection = CameraRotation.Vector(); // This gives us the direction the camera is facing
	float magnitude = 300.f;

	FVector LeftVector = Quat1.RotateVector(CameraDirection);
	FVector RightVector = Quat2.RotateVector(CameraDirection);

	// leftVector rotated to angle
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter); // Ignore the character itself
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacter->GetActorLocation(),
													 OwnerCharacter->GetActorLocation() + RightVector * magnitude,
													 ECC_Visibility, QueryParams);

	// rightVector rotated to angle
	FHitResult HitResult1;
	FCollisionQueryParams QueryParams1;
	QueryParams.AddIgnoredActor(OwnerCharacter); // Ignore the character itself
	bool bHit1 = GetWorld()->LineTraceSingleByChannel(HitResult1, OwnerCharacter->GetActorLocation(),
													  OwnerCharacter->GetActorLocation() + LeftVector * magnitude,
													  ECC_Visibility, // Or use another channel like ECC_Pawn
													  QueryParams1);

	/*
   * Debugging
   */
	// DebugLineTrace(HitResult, bHit,
	// FColor::Green,OwnerCharacter->GetActorLocation(),
	// OwnerCharacter->GetActorLocation() + RightVector * magnitude);
	// DebugLineTrace(HitResult1, bHit1, FColor::Yellow,
	// OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() +
	// LeftVector * magnitude);
	/*
   * Debugging
   */

	return !(bHit && bHit1);
}

void USR_WallRunComponent::StopWallRun()
{
	bIsStateActive = false;
	WallRunFallingSpeed = 0;
	WallRunSide = 0;
	bIsWallRunningLeft = false;

	StartCameraRoll = CurrentCameraRoll;
	CameraResetTimer = 0.0f;
	bResettingCamera = true;

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	ContextStateComponent->TransitionState(MotionState::NONE, true);
}

void USR_WallRunComponent::UpdateState(float deltaTime)
{
	WallRunFallingSpeed += WallRunFallingAcceleration * deltaTime;
	FVector CurrentVelocity = CharacterMovement->Velocity;
	float CurrentSpeed = FVector::DotProduct(CurrentVelocity, WallRunDirection);

	float WallRunSpeed = FMath::Clamp(CurrentSpeed, MinWallRunClampedSpeed, MaxWallRunClampedSpeed);
	CharacterMovement->Velocity = WallRunDirection * WallRunSpeed;

	// controlled gravity
	CharacterMovement->Velocity.Z = CharacterMovement->Velocity.Z - WallRunFallingSpeed;

	FVector Delta = CharacterMovement->Velocity * deltaTime;
	FHitResult Hit(1.f);

	CharacterMovement->SafeMoveUpdatedComponent(Delta, LockedRotation,
												true, Hit);

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
		// CharacterMovement->SafeMoveUpdatedComponent(Delta,
		// CharacterMovement->UpdatedComponent->GetComponentRotation(), true, Hit);
		// // ?? what is it for ??
	}

	auto CharacterForwardVector = OwnerCharacter->GetActorForwardVector();
	auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
	auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	if (angle > MaxAngleBeforeStop)
	{
		LeaveState(-1, true);
	}
}

void USR_WallRunComponent::OnJumpButtonPressed()
{
	if (!bIsStateActive)
	{
		return;
	}
	LeaveState(-1);
	FWallJumpData WallJumpData;
	WallJumpData.WallRunDirection = WallRunDirection;
	WallJumpData.WallNormal = WallNormal;
	void *Data = &WallJumpData;
	ContextStateComponent->TransitionState(MotionState::WALL_JUMP, Data, true);
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void USR_WallRunComponent::OnJumpButtonReleased() {}

void USR_WallRunComponent::DebugLineTrace(FHitResult hitResult, bool hit, FColor color, FVector vectorStart,
										  FVector vectorEnd)
{
	DrawDebugLine(GetWorld(), vectorStart, vectorEnd, color, false, 0.1f, 0, 1.f);
	if (hit)
	{
		// Draw a point at the impact point
		DrawDebugPoint(GetWorld(), hitResult.ImpactPoint,
					   10.0f, // Size of the point
					   color, // Color on hit
					   false, // Persistent
					   5.0f // Duration
		);
	}
}
