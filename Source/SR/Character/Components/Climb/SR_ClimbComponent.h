// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/SR_Character.h"
#include "SR_ClimbComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClimbStarted);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_ClimbComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_ClimbComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;

	virtual void EnterState(void *Data) override;
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",
			  meta = (ExposeOnSpawn = true, ToolTip = "Maximum distance at which character can reach for a surface"))
	float ReachDistance = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb",
			  meta = (ExposeOnSpawn = true, ToolTip = "Strength of forward impulse when climbing"))
	float ForwardImpulseStrength = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb",
			  meta = (ExposeOnSpawn = true, ToolTip = "Upward component of climbing movement"))
	float UpwardValue = 0.2f;

	UFUNCTION()
	void OnMoveForwardInputPressed();
	UFUNCTION()
	void OnMoveForwardInputReleased();

	UPROPERTY(BlueprintAssignable, Category = "Climb Events")
	FOnClimbStarted OnClimbStarted;

	UFUNCTION(BlueprintCallable, Category = "Climb")
	bool IsClimbActive() const { return bIsActive; }

private:
	UPROPERTY()
	UCharacterMovementComponent *CharacterMovement;

	UPROPERTY()
	ASR_Character *OwnerCharacter;

	UPROPERTY()
	USR_MotionController *MotionController;

	UPROPERTY()
	USR_ContextStateComponent *ContextStateComponent;

	FVector StartLocation = FVector::ZeroVector;

	bool bIsActive = false;

	void CheckForLedgeGrab();

	UPROPERTY(EditDefaultsOnly, Category = "Climb", meta = (ToolTip = "Maximum distance for ledge grab detection"))
	float LedgeGrabReachDistance = 70.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Climb", meta = (ToolTip = "Vertical grab height for ledge detection"))
	float LedgeGrabHeight = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Climb", meta = (ToolTip = "Speed at which character climbs upward"))
	float ClimbUpSpeed = 20.0f;

	FVector LedgeLocation;

	bool bIsMovingForward = false;
};
