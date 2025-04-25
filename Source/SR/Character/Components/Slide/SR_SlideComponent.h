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
	// Sets default values for this component's properties
	USR_SlideComponent();


	UFUNCTION()
	void Slide();
	
	/**
	 * @description : Call when player slide input is pressed
	 * @name : StartSlide
	 * @param 
	 */
	UFUNCTION(BlueprintCallable)
	void StartSlide();

	/**
	 * @description : Call when player slide input is release
	 * @name : StopSlide
	 * @param 
	 */
	UFUNCTION()
	void StopSlide();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsSliding = false;
	bool bIsCrouching = false;

	virtual void EnterState(void* data) override;
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY()
	float FGravity = 0.0f;

	UPROPERTY()
	float FFriction = 0.0f;

	UPROPERTY()
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	UMeshComponent* MeshComponent;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;
	
	UPROPERTY(BlueprintReadWrite, Category = "Slide Movement")
	UCurveFloat* CurveFloat = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FSlideDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement")
	float FSlideSpeed = 200.0f;

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

	float GetCurrentFloorAngle();

	float CalculateSlideSpeed(float DeltaTime);
	
	UPROPERTY()
	ASR_Character* OwnerCharacter;
	UPROPERTY()
	USR_MotionController* MotionController;
	UPROPERTY()
	USR_ContextStateComponent* ContextStateComponent;
	

	int32 m_CurrentRootMotionID = 0;

	FVector SlideStartLocation;
	FVector SlideDirection;

	float FOriginalFriction = 0.0f;
	float FOriginalBrakingFriction = 0.0f;
	
	bool bIsStateActive = false;
	
	float FInitialCapsuleHalfHeight = 96.0f;
	float CurrentSlideDistance = 0.0f;
};