// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_SlideComponent.h"


// Sets default values for this component's properties
USR_SlideComponent::USR_SlideComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USR_SlideComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void USR_SlideComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessSlide(DeltaTime);
}

void USR_SlideComponent::StartSlide()
{
	if (CanInitiateSlide())
	{
		InitializeSlideState();
		UpdateSlideDirection();
		AdjustCharacterCollision();
	}
	else
	{
		HandleCrouchFallback();
	}
}

bool USR_SlideComponent::CanInitiateSlide() const
{
	return !bIsSliding && 
		   CharacterMovement->GetLastUpdateVelocity() != FVector::ZeroVector && 
		   !CharacterMovement->IsFalling();
}

void USR_SlideComponent::InitializeSlideState()
{
	bIsSliding = true;
	SlideStartLocation = GetOwner()->GetActorLocation();
	SlideDirection = GetOwner()->GetActorForwardVector();
	SlideSpeed = CharacterMovement->Velocity.Size();
	CurrentSlideDistance = 0.0f;
}

void USR_SlideComponent::UpdateSlideDirection()
{
	FHitResult HitResult;
	if (PerformGroundCheck(HitResult))
	{
		FVector PlayerForward = GetOwner()->GetActorForwardVector();
		SlideDirection = FVector::VectorPlaneProject(PlayerForward, HitResult.Normal).GetSafeNormal();
	}
	else
	{
		SlideDirection = GetOwner()->GetActorForwardVector();
	}
}

bool USR_SlideComponent::PerformGroundCheck(FHitResult& OutHitResult) const
{
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 100.0f);
    
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
    
	return GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility, QueryParams);
}

void USR_SlideComponent::AdjustCharacterCollision()
{
	if (CapsuleComponent && MeshComponent)
	{
		CapsuleComponent->SetCapsuleHalfHeight(fCapsuleHalfHeightSliding);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
	}
}

void USR_SlideComponent::HandleCrouchFallback()
{
	if (CharacterMovement->GetLastUpdateVelocity() == FVector::ZeroVector && 
		!CharacterMovement->IsFalling())
	{
		CharacterMovement->Crouch();
	}
}

void USR_SlideComponent::ProcessSlide(float DeltaTime)
{
    if (!bIsSliding)
    {
        return;
    }

    float FrameDistance = SlideSpeed * DeltaTime;
    
    UpdateSlideDistance(FrameDistance);
    
    if (ShouldStopSlide())
    {
        StopSlide();
        return;
    }

    if (!UpdateSlidePosition(DeltaTime))
    {
        StopSlide();
    }
}

float USR_SlideComponent::CalculateCurrentSlideSpeed() const
{
    FHitResult GroundHit;
    if (!PerformGroundCheck(GroundHit))
    {
        return SlideSpeed;
    }

    return SlideSpeed * CalculateSpeedMultiplierFromSlope(GroundHit);
}

float USR_SlideComponent::CalculateSpeedMultiplierFromSlope(const FHitResult& GroundHit) const
{
    float SlopeAngle = FMath::RadiansToDegrees(
        FMath::Acos(FVector::DotProduct(GroundHit.Normal, FVector::UpVector))
    );

    if (SlopeAngle < 45.0f)
    {
        return FMath::GetMappedRangeValueClamped(
            FVector2D(0.0f, 45.0f),
            FVector2D(0.2f, 1.0f),
            SlopeAngle
        );
    }

    return 1.0f;
}

void USR_SlideComponent::UpdateSlideDistance(float FrameDistance)
{
    CurrentSlideDistance += FrameDistance;
}

bool USR_SlideComponent::ShouldStopSlide() const
{
    return CurrentSlideDistance >= SlideDistance;
}

bool USR_SlideComponent::UpdateSlidePosition(float DeltaTime)
{
    FVector NewLocation = GetOwner()->GetActorLocation() + 
                         (SlideDirection * SlideSpeed * DeltaTime);
    
    if (!CheckCollisionAtNewPosition(NewLocation))
    {
        GetOwner()->SetActorLocation(NewLocation);
        return true;
    }
    
    return false;
}

bool USR_SlideComponent::CheckCollisionAtNewPosition(const FVector& NewLocation) const
{
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());

    return GetWorld()->LineTraceSingleByChannel(
        HitResult,
        GetOwner()->GetActorLocation(),
        NewLocation,
        ECC_Visibility,
        CollisionParams
    );
}


void USR_SlideComponent::StopSlide()
{
	bIsSliding = false;
	CapsuleComponent->SetCapsuleHalfHeight(fInitialCapsuleHalfHeight);
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90));
}