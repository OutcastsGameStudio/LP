// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_SlideComponent.h"

// Sets default values for this component's properties
USR_SlideComponent::USR_SlideComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CapsuleComponent = nullptr;
	MeshComponent = nullptr;
	CharacterMovement = nullptr;
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
	FSlideSpeed = CharacterMovement->Velocity.Size();
	CurrentSlideDistance = 0.0f;
}

void USR_SlideComponent::UpdateSlideDirection()
{
	FHitResult HitResult;
	if (PerformGroundCheck(HitResult))
	{
		FVector const FPlayerForward = GetOwner()->GetActorForwardVector();
		SlideDirection = FVector::VectorPlaneProject(FPlayerForward, HitResult.Normal).GetSafeNormal();
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
		CapsuleComponent->SetCapsuleHalfHeight(FCapsuleHalfHeightSliding);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CapsuleHalfHeight: ") + FString::FromInt(CapsuleComponent->GetScaledCapsuleHalfHeight()));
	}
}

void USR_SlideComponent::HandleCrouchFallback()
{
	if (CharacterMovement->GetLastUpdateVelocity() == FVector::ZeroVector && 
		!CharacterMovement->IsFalling())
	{
		bIsCrouching = true;
		CharacterMovement->Crouch();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CapsuleHalfHeight: ") + FString::FromInt(CapsuleComponent->GetScaledCapsuleHalfHeight()));
	}
}

void USR_SlideComponent::ProcessSlide(float DeltaTime)
{
    if (!bIsSliding && !bIsCrouching)
    {
        return;
    }

    float const FrameDistance = FSlideSpeed * DeltaTime;
    
    UpdateSlideDistance(FrameDistance);
    
    if (CurrentSlideDistance >= FSlideDistance)
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
        return FSlideSpeed;
    }

    return FSlideSpeed * CalculateSpeedMultiplierFromSlope(GroundHit);
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

bool USR_SlideComponent::UpdateSlidePosition(float DeltaTime)
{
    FVector NewLocation = GetOwner()->GetActorLocation() + 
                         (SlideDirection * FSlideSpeed * DeltaTime);
	
	/**
	 * If new location is not colliding with anything, move the character to the new location
	 */
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
	if (!CharacterMovement || !CapsuleComponent)
	{
		return;
	}

	bIsSliding = false;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
    
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + FVector(0, 0, FInitialCapsuleHalfHeight * 2.0f);
    
	FHitResult HitResult;
	bool bHitCeiling = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	if (bHitCeiling)
	{
		bIsCrouching = true;
		CharacterMovement->bWantsToCrouch = true;
		CharacterMovement->Crouch();
	}
	else
	{
		bIsCrouching = false;
		CharacterMovement->bWantsToCrouch = false;
        
		if (CapsuleComponent->GetUnscaledCapsuleHalfHeight() < FInitialCapsuleHalfHeight)
		{
			CharacterMovement->UnCrouch();
		}
	}
    
	if (MeshComponent)
	{
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	}
}