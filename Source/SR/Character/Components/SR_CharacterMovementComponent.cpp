// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_CharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
USR_CharacterMovementComponent::USR_CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USR_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterOwner()->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &USR_CharacterMovementComponent::OnHit);
}


// Called every frame
void USR_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CanWallRun();
}

void USR_CharacterMovementComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (MovementMode == MOVE_Falling && Velocity.Z < 0.f && FMath::IsNearlyZero(Hit.Normal.Z) && CanWallRun())
	{
		auto CharacterForwardVector = GetCharacterOwner()->GetActorForwardVector();
		auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
		auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
		if(angle > MaxAngleWallRun || angle <= 15 ) return;
		SetMovementMode(MOVE_Custom, CUSTOM_WallRun);
		m_WallNormal = Hit.Normal;
		FVector WallDirection = FVector::CrossProduct(FVector::UpVector, Hit.Normal);
		m_WallRunDirection = WallDirection * (GetCharacterOwner()->GetActorForwardVector().Dot(WallDirection) > 0.f ? 1.f : -1.f);
	}
}

void USR_CharacterMovementComponent::PhysWallRun(float deltaTime, int32 Iterations)
{
	WallRunFallingSpeed += WallRunFallingAcceleration * deltaTime;
	Velocity = m_WallRunDirection * MaxWalkSpeed;

	Velocity.Z = Velocity.Z - WallRunFallingSpeed;
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Velocity : %s"), *Velocity.ToString()));
	}


	FVector Delta = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	if (Hit.IsValidBlockingHit() && Hit.Normal.Z > 0.f)
	{
		if (Hit.Normal.Z > 0.5f)
		{
			SetMovementMode(MOVE_Walking, 0);
		}
	}
	else if (!DetectNextWall(Hit))
	{
		SetMovementMode(MOVE_Falling);
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
	}
	auto CharacterForwardVector = GetCharacterOwner()->GetActorForwardVector();
	auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
	auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	if(angle > MaxAngleBeforeStop) SetMovementMode(MOVE_Falling);
}

void USR_CharacterMovementComponent::StopWallJump()
{
	if (MovementMode != MOVE_Custom) return;
	Velocity = (m_WallRunDirection + m_WallNormal + FVector::UpVector).GetSafeNormal() * WallJumpSpeed;
	SetMovementMode(MOVE_Falling);
}


void USR_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	switch(CustomMovementMode)
	{
	case CUSTOM_WallRun:
		PhysWallRun(deltaTime, Iterations);
		break;
	case CUSTOM_DASH:
		break;
	case CUSTOM_None:
		break;
	default:
		break;
	}
}

bool USR_CharacterMovementComponent::DetectNextWall(FHitResult& Hit)
{
	UCapsuleComponent* Capsule = GetCharacterOwner()->GetCapsuleComponent();
	FName CollisionProfile = Capsule->GetCollisionProfileName();
	FVector Start = GetCharacterOwner()->GetActorLocation();
	FVector End = Start - m_WallNormal * Capsule->GetScaledCapsuleRadius();
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(GetCharacterOwner());
	GetWorld()->SweepSingleByProfile(Hit, Start, End, Capsule->GetComponentQuat(), CollisionProfile, Capsule->GetCollisionShape(), Params);
	FVector Test = Start - m_WallNormal * Capsule->GetScaledCapsuleRadius();
	Test = Capsule->GetComponentQuat().RotateVector(Test);
	DrawDebugLine(GetWorld(), Start, Test, FColor::Green, false, 0.1f, 0, 1.f);
	return Hit.bBlockingHit;
}

bool USR_CharacterMovementComponent::CanWallRun()
{
	float rotation = 45.f;
	FRotator Rotator1(0.f, rotation, 0.f);
	FRotator Rotator2(0.f, -rotation, 0.f);

	FQuat Quat1(Rotator1);
	FQuat Quat2(Rotator2);
	APlayerController* PC = Cast<APlayerController>(GetCharacterOwner()->GetController());

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
	QueryParams.AddIgnoredActor(GetCharacterOwner());  // Ignore the character itself
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetCharacterOwner()->GetActorLocation(),
		GetCharacterOwner()->GetActorLocation() + RightVector * magnitude,
		ECC_Visibility,
		QueryParams
	);

	//rightVector rotated to angle
	FHitResult HitResult1;
	FCollisionQueryParams QueryParams1;
	QueryParams.AddIgnoredActor(GetCharacterOwner());  // Ignore the character itself
	bool bHit1 = GetWorld()->LineTraceSingleByChannel(
		HitResult1,
		GetCharacterOwner()->GetActorLocation(),
		GetCharacterOwner()->GetActorLocation() + LeftVector * magnitude,
		ECC_Visibility,  // Or use another channel like ECC_Pawn
		QueryParams1
	);

	// DebugLineTrace(HitResult, bHit, FColor::Green,GetCharacterOwner()->GetActorLocation(), GetCharacterOwner()->GetActorLocation() + RightVector * magnitude);
	// DebugLineTrace(HitResult1, bHit1, FColor::Yellow, GetCharacterOwner()->GetActorLocation(), GetCharacterOwner()->GetActorLocation() + LeftVector * magnitude);

	return !(bHit && bHit1);
}

void USR_CharacterMovementComponent::DebugLineTrace(FHitResult hitResult, bool hit,FColor color, FVector vectorStart, FVector vectorEnd )
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
