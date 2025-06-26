// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Motion/SR_MotionController.h"

#include "SR_SlideComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlideStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlideEnded);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
	                           FActorComponentTickFunction *ThisTickFunction) override;

	virtual void EnterState(void *Data) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;

	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
	FOnSlideStarted OnSlideStarted;

	UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
	FOnSlideEnded OnSlideEnded;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Settings",
		meta = (ToolTip = "Maximum distance the character can slide"))
	float MaxSlideDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement", meta = (ToolTip = "Speed when sliding"))
	float SlideSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement",
		meta = (ToolTip = "Capsule half height when sliding"))
	float CapsuleHalfHeightSliding = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide Movement",
		meta = (ToolTip = "Z offset for mesh when sliding"))
	float MeshLocationZ = -90.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slide Movement",
		meta = (ToolTip = "Whether the character is currently sliding"))
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
	bool PerformGroundCheck(FHitResult &OutHitResult) const;

	/**
   * @description : Adjust Parameters of the character collision (Mesh and
   * Capsule)
   * @name : AdjustCharacterCollision
   */
	void AdjustCharacterCollision();

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
	bool CheckCollisionAtNewPosition(const FVector &NewLocation) const;

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
	ASR_Character *OwnerCharacter;

	UPROPERTY()
	USR_MotionController *MotionController;

	UPROPERTY()
	USR_ContextStateComponent *ContextStateComponent;

	UPROPERTY()
	UCapsuleComponent *CapsuleComponent;

	UPROPERTY()
	UMeshComponent *MeshComponent;

	UPROPERTY()
	UCharacterMovementComponent *CharacterMovement;

	float Gravity = 0.0f;
	float Friction = 0.0f;

	int32 CurrentRootMotionID = 0;

	FVector SlideStartLocation;
	FVector SlideDirection;

	bool bIsStateActive = false;
	bool bIsCrouching = false;

	float InitialCapsuleHalfHeight = 96.0f;
	float CurrentSlideDistance = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Slide Movement", meta = (ToolTip = "Rate at which basic slide speed decreases"))
	float BasicSlideDeceleration = 200.0f;
};
