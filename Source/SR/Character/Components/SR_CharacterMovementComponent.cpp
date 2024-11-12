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

	// Draw Line Trace for Wallrun
	FVector Start = GetCharacterOwner()->GetActorLocation();
	FVector End = Start + GetCharacterOwner()->GetActorForwardVector() * 200.f;
	End.Y = End.Y + 45.f;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f);
}

void USR_CharacterMovementComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (MovementMode == MOVE_Falling && Velocity.Z < 0.f && FMath::IsNearlyZero(Hit.Normal.Z))
	{
		UE_LOGFMT(LogTemp, Warning, "Start Wallrun detected");

		SetMovementMode(MOVE_Custom, 0);
		WallNormal = Hit.Normal;
		FVector WallDirection = FVector::CrossProduct(FVector::UpVector, Hit.Normal);
		WallRunDirection = WallDirection * (GetCharacterOwner()->GetActorForwardVector().Dot(WallDirection) > 0.f ? 1.f : -1.f);
	}
}

void USR_CharacterMovementComponent::WallJump()
{
	if (MovementMode == MOVE_Custom)
	{
		Velocity = (WallRunDirection + WallNormal + FVector::UpVector).GetSafeNormal() * WallJumpSpeed;
		SetMovementMode(MOVE_Falling);
	}
}


void USR_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	LastUpdateVelocity = Velocity;
	Velocity = WallRunDirection * MaxWalkSpeed;

	FVector Delta = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	if (Hit.IsValidBlockingHit())
    {
    	if (FMath::IsNearlyZero(Hit.Normal.Z))
    	{
    		SetMovementMode(MOVE_Walking, 0);
    		return;
    	}
    }
	else if (DetectNextWall(Hit))
	{
		SetMovementMode(MOVE_Falling);
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
	}
}

void USR_CharacterMovementComponent::UpdateWallRunDirection(FHitResult& Hit)
{
	//On utilise la rotation entre le vecteur normal du mur précédent et le vecteur normal du hit (nouveau mur) pour calculer la nouvelle direction
	FQuat Rotation = FQuat::FindBetweenNormals(WallNormal, Hit.Normal);
	WallNormal = Hit.Normal;
	WallRunDirection = Rotation.RotateVector(WallRunDirection);
}

bool USR_CharacterMovementComponent::DetectNextWall(FHitResult& Hit)
{
	UCapsuleComponent* Capsule = GetCharacterOwner()->GetCapsuleComponent();
	FName CollisionProfile = Capsule->GetCollisionProfileName();
	
	//On fait un capsule collision avec les infos de la capsule du personnage.
	//Puis on fait un sweep vers le mur
	FVector Start = GetCharacterOwner()->GetActorLocation();
	FVector End = Start - WallNormal * Capsule->GetScaledCapsuleRadius();
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(GetCharacterOwner());
	
	GetWorld()->SweepSingleByProfile(Hit, Start, End, Capsule->GetComponentQuat(), CollisionProfile, Capsule->GetCollisionShape(), Params);

	return Hit.bBlockingHit;
}

void USR_CharacterMovementComponent::Dash(FVector DashDirection)
{
	if (MovementMode == MOVE_Walking)
	{
		GravityScale = 0.f;
		BrakingDecelerationFalling = 0.f;
		Velocity = DashDirection.GetSafeNormal() * 10000.f;
		GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &USR_CharacterMovementComponent::EndDash, 0.2f, false);
	}
	else if (MovementMode == MOVE_Falling)
	{
		GravityScale = 0.f;
		BrakingDecelerationFalling = 0.f;
		Velocity = DashDirection.GetSafeNormal() * 2000.f;
		GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &USR_CharacterMovementComponent::EndDash, 0.2f, false);
	}
}

void USR_CharacterMovementComponent::EndDash()
{
	GravityScale = 2.f;
	BrakingDecelerationFalling = 1500.f;
}

