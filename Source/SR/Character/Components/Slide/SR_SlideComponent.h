// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_SlideComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_SlideComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_SlideComponent();

	/**
	 * @description : Call when player slide input is pressed
	 * @name : StartSlide
	 * @param 
	 */
	void StartSlide();

	/**
	 * @description : Call when player slide input is release
	 * @name : StopSlide
	 * @param 
	 */
	void StopSlide();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsSliding = false;
	bool bIsCrouching = false;

	// TODO : Cast to USR_CharacterMovementComponent for garbage collection
	UCapsuleComponent* CapsuleComponent;
	UMeshComponent* MeshComponent;
	UCharacterMovementComponent* CharacterMovement;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FSlideDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FSlideDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FSlideSpeed = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FCapsuleHalfHeightSliding = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FMeshLocationZ = -90.0f;

private:
	/**
	 * @description : Check if different conditions are met to start the slide
	 * @name : CanInitiateSlide
	 */
	bool CanInitiateSlide() const;

	/**
	 * @description : Set the differents param of the slide movement
	 * @name : InitializeSlideState
	 */
	void InitializeSlideState();

	/**
	 * @description : Check if the character is on the ground
	 * @name : UpdateSlideDirection
	 */
	void UpdateSlideDirection();

	/**
	 * @description : Check if the character is on the ground
	 * @name : PerformGroundCheck
	 * @parame : OutHitResult
	 */
	bool PerformGroundCheck(FHitResult& OutHitResult) const;

	/**
	 * @description : Adjust Parameters of the character collision (Mesh and Capsule)
	 * @name : AdjustCharacterCollision
	 */
	void AdjustCharacterCollision();

	/**
	 * @description : When stop the slide and velocity is null, crouch the character
	 * @name : HandleCrouchFallback
	 */
	void HandleCrouchFallback();

	/**
	 * @description : Physics custom for slide movement
	 * @name : ProcessSlide
	 * @param DeltaTime
	 */
	void ProcessSlide(float DeltaTime);

	/**
	 * @description : Calculate the current speed of the slide
	 * @name : CalculateCurrentSlideSpeed
	 */
	float CalculateCurrentSlideSpeed() const;

	/**
	 * @description : Calculate the speed multiplier from the slope
	 * @name : CalculateSpeedMultiplierFromSlope
	 * @param GroundHit
	 */
	float CalculateSpeedMultiplierFromSlope(const FHitResult& GroundHit) const;

	/**
	 * @description : Update the slide position
	 * @name : UpdateSlidePosition
	 * @param DeltaTime
	 */
	bool UpdateSlidePosition(float DeltaTime);

	/**
	 * @description : Check if the new location is not colliding with anything
	 * @name : CheckCollisionAtNewPosition
	 * @param NewLocation
	 */
	bool CheckCollisionAtNewPosition(const FVector& NewLocation) const;

	/**
	 * @description : Update the slide distance
	 * @name : UpdateSlideDistance
	 * @param FrameDistance
	 */
	void UpdateSlideDistance(float FrameDistance);

	FVector SlideStartLocation;
	FVector SlideDirection;
	float FInitialCapsuleHalfHeight = 96.0f;
	float CurrentSlideDistance = 0.0f;
};