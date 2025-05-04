#include "SR_BridgePlatform.h"


ASR_BridgePlatform::ASR_BridgePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	RootComponent = PlatformMesh;
    
	bIsMoving = false;
	bShouldRotate = false;
	MovementSpeed = 1.0f;
	Direction = 1.0f;
}

void ASR_BridgePlatform::BeginPlay()
{
	Super::BeginPlay();
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
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("FIRST"));

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
			StartRotation = GetActorRotation();
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("SECOND"));
			TargetRotation = StartRotation - EndRotation;
		} else
		{
			TargetPosition = StartPosition;
			StartPosition = GetActorLocation();
		}
	}
}
