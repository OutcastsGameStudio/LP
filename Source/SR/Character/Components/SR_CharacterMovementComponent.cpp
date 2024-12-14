// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_CharacterMovementComponent.h"

#include "CollisionDebugDrawingPublic.h"
#include "KismetTraceUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Logging/StructuredLog.h" 


// Sets default values for this component's properties
USR_CharacterMovementComponent::USR_CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
}

void USR_CharacterMovementComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (MovementMode == MOVE_Falling && Velocity.Z < 0.f && FMath::IsNearlyZero(Hit.Normal.Z))
	{
		auto CharacterForwardVector = GetCharacterOwner()->GetActorForwardVector();
		auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
		auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
		if(angle > MaxAngleWallRun) return;
		SetMovementMode(MOVE_Custom, CUSTOM_WallRun);
		m_WallNormal = Hit.Normal;
		FVector WallDirection = FVector::CrossProduct(FVector::UpVector, Hit.Normal);
		m_WallRunDirection = WallDirection * (GetCharacterOwner()->GetActorForwardVector().Dot(WallDirection) > 0.f ? 1.f : -1.f);
	}
}

void USR_CharacterMovementComponent::PhysWallRun(float deltaTime, int32 Iterations)
{
	LastUpdateVelocity = Velocity;
	Velocity = m_WallRunDirection * MaxWalkSpeed;

	FVector Delta = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	if (Hit.IsValidBlockingHit() && Hit.Normal.Z > 0.f)
	{
		if (Hit.Normal.Z > 0.5f)
		{
			SetMovementMode(MOVE_Walking, 0);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("End wall run detected !"));
		}
	}
	else if (!DetectNextWall(Hit))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Next wall detected !"));
		SetMovementMode(MOVE_Falling);
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
	}
	auto CharacterForwardVector = GetCharacterOwner()->GetActorForwardVector();
	auto DotProduct = FVector::DotProduct(CharacterForwardVector, -Hit.Normal);
	auto angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	if(angle > MaxAngleWallRun) SetMovementMode(MOVE_Falling);
}

void USR_CharacterMovementComponent::StopWallJump()
{
	if (MovementMode != MOVE_Custom) return
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Wall jump"));
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

void USR_CharacterMovementComponent::UpdateWallRunDirection(FHitResult& Hit)
{
	FQuat Rotation = FQuat::FindBetweenNormals(m_WallNormal, Hit.Normal);
	m_WallNormal = Hit.Normal;
	m_WallRunDirection = Rotation.RotateVector(m_WallRunDirection);
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
	return Hit.bBlockingHit;
}
