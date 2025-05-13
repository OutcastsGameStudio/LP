// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Motion/SR_MotionController.h"

#include "SR_SlideComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_SlideComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	USR_SlideComponent();
	
	/**
	 * @description : Call when player press the slide input
	 * @name : Slide
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Slide();
	
	/**
	 * @description : Call when state is activated and can be used
	 * @name : StartSlide
	 */
	UFUNCTION()
	void StartSlide();

	/**
	 * @description : Call when slide should stop
	 * @name : StopSlide
	 */
	UFUNCTION()
	void StopSlide();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EnterState(void* data) override;
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Settings")
	float MaxSlideDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FSlideSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FCapsuleHalfHeightSliding = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FMeshLocationZ = -90.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slide Movement")
	bool bIsSliding = false;
	
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
	FVector UpdateSlideDirection();

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
	 * @description : Check if the new location is not colliding with anything
	 * @name : CheckCollisionAtNewPosition
	 * @param NewLocation
	 */
	bool CheckCollisionAtNewPosition(const FVector& NewLocation) const;

	/**
	 * @description : Get the floor angle where the player is sliding
	 * @name : GetCurrentFloorAngle
	 */
	float GetCurrentFloorAngle();

	/**
	 * @description : Calculate the slide speed based on the floor angle
	 * @name : CalculateSlideSpeed
	 * @param DeltaTime
	 */
	float CalculateSlideSpeed(float DeltaTime);

	/**
	 * @description : Process slide movement when the floor is flat
	 * @name : ProcessBasicSlide
	 * @param DeltaTime
	 */
	float ProcessBasicSlide(float DeltaTime);
	
	UPROPERTY()
	ASR_Character* OwnerCharacter;
	
	UPROPERTY()
	USR_MotionController* MotionController;
	
	UPROPERTY()
	USR_ContextStateComponent* ContextStateComponent;

	UPROPERTY()
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	UMeshComponent* MeshComponent;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;
	
	float FGravity = 0.0f;
	float FFriction = 0.0f;
	
	int32 m_CurrentRootMotionID = 0;

	FVector SlideStartLocation;
	FVector SlideDirection;
	
	bool bIsStateActive = false;
	bool bIsCrouching = false;
	
	float FInitialCapsuleHalfHeight = 96.0f;
	float CurrentSlideDistance = 0.0f;
	float BasicSlideDeceleration = 200.0f;
};