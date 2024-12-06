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
	if (!bIsSliding && CharacterMovement->GetLastUpdateVelocity() != FVector::ZeroVector && !CharacterMovement->IsFalling())
	{
		bIsSliding = true;
		SlideStartLocation = GetOwner()->GetActorLocation();
		SlideDirection = GetOwner()->GetActorForwardVector();

		CurrentSlideDistance = 0.0f;

		if (CapsuleComponent && MeshComponent)
		{
			CapsuleComponent->SetCapsuleHalfHeight(fCapsuleHalfHeightSliding);
			MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -45));
		}
	} else if (CharacterMovement->GetLastUpdateVelocity() == FVector::ZeroVector && !CharacterMovement->IsFalling())
	{
		CharacterMovement->Crouch();
	}
}

void USR_SlideComponent::ProcessSlide(float DeltaTime)
{
	if (bIsSliding)
	{
		float FrameDistance = SlideSpeed * DeltaTime;

		CurrentSlideDistance += FrameDistance;

		if (CurrentSlideDistance >= SlideDistance)
		{
			StopSlide();
			return;
		}

		FVector NewLocation = GetOwner()->GetActorLocation() + (SlideDirection * FrameDistance);

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());

		bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				GetOwner()->GetActorLocation(),
				NewLocation,
				ECC_Visibility,
				CollisionParams
			);
		if (!bHit)
		{
			GetOwner()->SetActorLocation(NewLocation);
		}
		else
		{
			bIsSliding = false;

			if (CapsuleComponent && MeshComponent)
			{
				StopSlide();
			}
		}
	}
}


void USR_SlideComponent::StopSlide()
{
	if (CharacterMovement->GetLastUpdateVelocity() == FVector::ZeroVector)
	{
		CharacterMovement->UnCrouch();
	} else
	{
		bIsSliding = false;
		CapsuleComponent->SetCapsuleHalfHeight(fInitialCapsuleHalfHeight);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90));
	}
}
