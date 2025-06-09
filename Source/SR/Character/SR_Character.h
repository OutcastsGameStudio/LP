// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ContextState/SR_ContextStateComponent.h"
#include "Components/Debug/SR_DebugComponent.h"
#include "Components/Interaction/SR_InteractionComponent.h"
#include "Components/SR_CharacterMovementComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Motion/SR_MotionController.h"
#include "SR_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ASR_PanelControl;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashInputPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashInputReleased);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveForwardInputPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveForwardInputReleased);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpInputPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpInputReleased);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlideInputPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlideInputReleased);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveInputChanged, FVector2D, Value);

UCLASS()
class SR_API ASR_Character : public ACharacter
{
	GENERATED_BODY()

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;

	/** ForwardAction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *ForwardAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *DashAction;

	/** Slide Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *SlideAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *UInteractAction;

public:
	ASR_Character();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnDashInputPressed OnDashInputPressed;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnDashInputReleased OnDashInputReleased;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnMoveForwardInputPressed OnMoveForwardInputPressed;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnMoveForwardInputReleased OnMoveForwardInputReleased;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnMoveForwardInputPressed FOnJumpInputPressed;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnMoveForwardInputReleased FOnJumpInputReleased;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnSlideInputReleased FOnSlideInputReleased;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnSlideInputPressed FOnSlideInputPressed;

	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnMoveInputChanged FOnMoveInputChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bRotateCharacterWithCamera = true;

	void SetCharacterLedgeGrabReachDistance(float NewReachDistance) { LedgeGrabReachDistance = NewReachDistance; }

	float GetCharacterLedgeGrabReachDistance() const { return LedgeGrabReachDistance; }

	void SetLedgeGrabHeight(float NewLedgeGrabHeight);

	float GetLedgeGrabHeight() const;

	void SetClimbUpSpeed(float NewClimbUpSpeed) { ClimbUpSpeed = NewClimbUpSpeed; }

	float GetClimbUpSpeed() const { return ClimbUpSpeed; }

	virtual void Jump() override;

	void MoveForward();

	void StopMoveForward();

	/**
   * @description : Set the custom movement mode of the character
   * @param NewCustomMode
   */
	void SetCharacterMovementCustomMode(USR_CharacterMovementComponent::CustomMode NewCustomMode);

	USR_DebugComponent *GetDebugComponent() const { return DebugComponent; }

protected:
	FVector LedgeLocation;

	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	/** Called for looking input */
	void Look(const FInputActionValue &Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	class USR_DashComponent *DashComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	class USR_WallRunComponent *WallRunComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	class USR_WallJumpComponent *WallJumpComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	class USR_ClimbComponent *ClimbComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration")
	class USR_SlideComponent *SlideComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	class USR_EnergyComponent *EnergyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USR_InteractionComponent *InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	USR_DebugComponent *DebugComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual void BeginPlay() override;

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Acceleration")
	USR_CharacterMovementComponent *m_CharacterMovementComponent;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ActivatePanel();

	UPROPERTY()
	ASR_PanelControl *NearestPanel;

	bool bPlatformMoving = false;

	float CameraSensitivity = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetCameraSensitivity(float NewCameraSensitivity) { CameraSensitivity = NewCameraSensitivity; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetCameraSensitivity() const { return CameraSensitivity; }

	/*
   * Slide Section.
   * Disables movement input when sliding but can use camera rotation.
   */
	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsMoveInputBlocked() const { return bBlockMovementInput; }

	void SetMoveInputBlocked(bool bBlocked) { bBlockMovementInput = bBlocked; };

private:
	/*
   * Dash Section
   */
	void OnDashPressed(const FInputActionValue &Value);
	void OnDashReleased(const FInputActionValue &Value);

	/*
   * WallMovement Section
   */
	void StopWallJump();

	/*
   * Slide Section
   */
	void OnSlidePressed();
	void OnSlideReleased();
	bool bBlockMovementInput = false;

	UPROPERTY()
	USR_ContextStateComponent *ContextStateComponent;

	UPROPERTY()
	USR_MotionController *MotionController;

	float LedgeGrabReachDistance = 70.0f;
	float LedgeGrabHeight = 150.0f;
	float ClimbUpSpeed = 20.0f;

public:
	ISR_State *GetState(MotionState StateName) const;

	//@TODO: workaround for the retrieving of the current State
	UFUNCTION(BlueprintCallable, Category = "State")
	FName GetCurrentStateName();
	void SetCurrentState(MotionState NewStateName) { CurrentState = NewStateName; }

private:
	MotionState CurrentState = MotionState::NONE;
};
