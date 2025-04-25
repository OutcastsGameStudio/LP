// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_SlideComponent.h"

#include "MaterialHLSLTree.h"
#include "SR/Character/SR_Character.h"

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

	OwnerCharacter = Cast<ASR_Character>(GetOwner());
	CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
	ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
	MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();
	CapsuleComponent = GetOwner()->FindComponentByClass<UCapsuleComponent>();
	MeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();

	FFriction = CharacterMovement->BrakingFriction;
	FGravity = 9.80f;


	if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_DashComponent::BeginPlay()"));
		return;
	}

	OwnerCharacter->FOnSlideInputPressed.AddDynamic(this, &USR_SlideComponent::Slide);
	OwnerCharacter->FOnSlideInputReleased.AddDynamic(this, &USR_SlideComponent::StopSlide);

	MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_SlideComponent::LeaveState);
}

void USR_SlideComponent::Slide()
{
	if (!CanInitiateSlide())
		HandleCrouchFallback();
	ContextStateComponent->TransitionState(MotionState::SLIDE);
}

void USR_SlideComponent::UpdateState()
{
	bIsStateActive = true;
}

void USR_SlideComponent::EnterState(void* data)
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
	if(rootMotionId != m_CurrentRootMotionID) return;
	
	if(!ContextStateComponent)
		UE_LOG(LogTemp, Error, TEXT("Failed to load ContextState in USR_SlideComponent::LeaveState()"));

	bIsStateActive = false;
	ContextStateComponent->TransitionState(MotionState::NONE, true);
}

bool USR_SlideComponent::LookAheadQuery()
{
	return !bIsStateActive;
}

FName USR_SlideComponent::GetStateName() const
{
	return FName("Slide");
}

int32 USR_SlideComponent::GetStatePriority() const
{
	return 0;
}

bool USR_SlideComponent::IsStateActive() const
{
	return bIsStateActive;
}

// Called every frame
void USR_SlideComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
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
	CharacterMovement->BrakingFriction = 0.0f;
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

FVector USR_SlideComponent::UpdateSlideDirection()
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

	return SlideDirection;
}

bool USR_SlideComponent::PerformGroundCheck(FHitResult& OutHitResult) const
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
	}
}

void USR_SlideComponent::ProcessSlide(float DeltaTime)
{
    if (!bIsSliding && !bIsCrouching)
        return;

	FSlideSpeed = CalculateSlideSpeed(DeltaTime);
	
	FVector NewLocation = GetOwner()->GetActorLocation() + 
						 (SlideDirection * FSlideSpeed * DeltaTime);
	
    if (CheckCollisionAtNewPosition(NewLocation))
        StopSlide();

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green,
		FString::Printf(TEXT("SLide speed : %f"), FSlideSpeed));

	GetOwner()->SetActorLocation(NewLocation);
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
		return;

	bIsSliding = false;

	CharacterMovement->BrakingFriction = 500.0f;

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

	ContextStateComponent->TransitionState(MotionState::NONE);
}

float USR_SlideComponent::GetCurrentFloorAngle()
{
	if (!OwnerCharacter)
		return 0.0f;
    
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
	
	FSlideSpeed = CharacterMovement->Velocity.Size();

	float GravityForce = FGravity * sin(CurrentFloorAngle);
	float resultGravityForce = GravityForce - FFriction * FGravity * cos(CurrentFloorAngle);

	float NewSlideSpeed = FSlideSpeed + resultGravityForce * DeltaTime * 10.0f;
	
	return NewSlideSpeed;
}