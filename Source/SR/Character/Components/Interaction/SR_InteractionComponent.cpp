#include "SR_InteractionComponent.h"
#include "../../SR_Character.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

USR_InteractionComponent::USR_InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	DetectedActor = nullptr;
}

void USR_InteractionComponent::BeginPlay() { Super::BeginPlay(); }

void USR_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											 FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FindInteractibleActorInView();
}

bool USR_InteractionComponent::IsActorInViewAndRange(const AActor *Actor, const FVector &CameraLocation,
													 const FVector &CameraForward, float &OutDistanceFromCenter)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	float Distance = FVector::Distance(CameraLocation, Actor->GetActorLocation());
	if (Distance > MaxDetectionDistance)
	{
		return false;
	}

	APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		return false;
	}

	FVector2D ScreenLocation;
	if (!PC->ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenLocation))
	{
		return false;
	}

	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

	FVector2D NormalizedScreenLoc(ScreenLocation.X / ViewportSizeX, ScreenLocation.Y / ViewportSizeY);

	OutDistanceFromCenter = FVector2D::Distance(NormalizedScreenLoc, FVector2D(0.5f, 0.5f));

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(Actor);

	bool bIsVisible = !GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Actor->GetActorLocation(),
															ECC_Visibility, QueryParams);

	return bIsVisible && OutDistanceFromCenter <= CenterScreenTolerance;
}

void USR_InteractionComponent::FindInteractibleActorInView()
{
	if (!IsValid(GetOwner()))
	{
		return;
	}

	ASR_Character *OwnerCharacter = Cast<ASR_Character>(GetOwner());
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	UCameraComponent *FollowCamera = OwnerCharacter->GetFollowCamera();
	if (!IsValid(FollowCamera))
	{
		return;
	}

	FVector CameraLocation = FollowCamera->GetComponentLocation();
	FVector CameraForward = FollowCamera->GetForwardVector();

	TArray<AActor *> InteractibleActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("interactible"), InteractibleActors);

	AActor *ClosestToCenterActor = nullptr;
	float ClosestDistanceToCenter = CenterScreenTolerance;

	for (AActor *Actor : InteractibleActors)
	{
		float DistanceFromCenter;
		if (IsActorInViewAndRange(Actor, CameraLocation, CameraForward, DistanceFromCenter))
		{
			if (DistanceFromCenter < ClosestDistanceToCenter)
			{
				ClosestDistanceToCenter = DistanceFromCenter;
				ClosestToCenterActor = Actor;
			}
		}
	}

	DetectedActor = ClosestToCenterActor;
}

AActor *USR_InteractionComponent::GetDetectedInteractibleActor() const { return DetectedActor; }
