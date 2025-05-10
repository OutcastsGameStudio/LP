#include "SR_BridgePlatform.h"

#include "Kismet/GameplayStatics.h"


ASR_BridgePlatform::ASR_BridgePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	RootComponent = PlatformMesh;
    
	bIsMoving = false;
	bShouldRotate = false;
	MovementSpeed = 1.0f;
}

void ASR_BridgePlatform::BeginPlay()
{
	Super::BeginPlay();
	OriginLocation = GetActorLocation();
	OriginRotation = GetActorRotation();
}

void ASR_BridgePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving)
		return;
	
	if (!bHasReachedDestination)
	{
		Alpha += MovementSpeed * DeltaTime;
        
		if (Alpha >= 1.0f)
		{
			Alpha = 1.0f;
			bHasReachedDestination = true;
			bIsMoving = false;
		}
        
		if (bShouldRotate)
		{
			FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
			SetActorRotation(NewRotation);
		}
		else
		{
			FVector NewLocation = FMath::Lerp(StartPosition, TargetPosition, Alpha);
			SetActorLocation(NewLocation);
		}
	}
}

void ASR_BridgePlatform::ActivateMovement(bool bShouldActivate)
{
	bIsReverse = bShouldActivate;
	bIsMoving = true;
	bHasReachedDestination = false;
	Alpha = 0.0f;

	if (bIsReverse)
	{
		if (bShouldRotate)
		{
			StartRotation = GetActorRotation();
			TargetRotation = StartRotation + EndRotation;
		} else
		{
			StartPosition = GetActorLocation();
			TargetPosition = StartPosition + EndPosition;
		}
	}
	else
	{
		if (bShouldRotate)
		{
			TargetRotation = StartRotation;
			StartRotation = TargetRotation + EndRotation;
		} else
		{
			TargetPosition = StartPosition;
			StartPosition = GetActorLocation();
		}
	}
	if (ActivationSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());
	}
}

void ASR_BridgePlatform::ResetPlatform()
{
	SetActorLocation(OriginLocation);
	SetActorRotation(OriginRotation);
}
