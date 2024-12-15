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

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsSliding = false;

	UCapsuleComponent* CapsuleComponent;
	UMeshComponent* MeshComponent;
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float fCapsuleHalfHeightSliding = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float fMeshLocationZ;
	
	float fInitialCapsuleHalfHeight = 96.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float SlideDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float SlideDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float SlideSpeed = 1800.0f;

private:
	FVector SlideStartLocation;
	FVector SlideDirection;
	float CurrentSlideDistance = 0.0f;

	bool CanInitiateSlide() const;
	void InitializeSlideState();
	void UpdateSlideDirection();
	bool PerformGroundCheck(FHitResult& OutHitResult) const;
	void AdjustCharacterCollision();
	void HandleCrouchFallback();

	/**
	 * @description : Set the differents param of the slide movement
	 * @name : ProcessSlide
	 * @param DeltaTime
	 */
	void ProcessSlide(float DeltaTime);
	
	float CalculateCurrentSlideSpeed() const;
	float CalculateSpeedMultiplierFromSlope(const FHitResult& GroundHit) const;
	bool UpdateSlidePosition(float DeltaTime);
	bool CheckCollisionAtNewPosition(const FVector& NewLocation) const;
	void UpdateSlideDistance(float FrameDistance);
	bool ShouldStopSlide() const;

};