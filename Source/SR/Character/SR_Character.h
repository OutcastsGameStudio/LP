// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SR_CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/Debug/SR_DebugComponent.h"
#include "SR_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class SR_API ASR_Character : public ACharacter
{
	GENERATED_BODY()

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Silde Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlideAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChrouchAction;


private:
	float LedgeGrabReachDistance = 70.0f;
	float LedgeGrabHeight = 150.0f;
	float ClimbUpSpeed = 20.0f;
	bool isCrouching = false;


public:
	ASR_Character();
	
	void Tick(float DeltaTime);

	void SetCharacterLedgeGrabReachDistance(float NewReachDistance) { LedgeGrabReachDistance = NewReachDistance; }

	float GetCharacterLedgeGrabReachDistance() const { return LedgeGrabReachDistance; }

	void SetLedgeGrabHeight(float NewLedgeGrabHeight);

	float GetLedgeGrabHeight() const;

	void SetClimbUpSpeed(float NewClimbUpSpeed) { ClimbUpSpeed = NewClimbUpSpeed; }

	float GetClimbUpSpeed() const { return ClimbUpSpeed; }

	/**
	 * @description : Set the custom movement mode of the character
	 * @param NewCustomMode 
	 */
	void SetCharacterMovementCustomMode(USR_CharacterMovementComponent::CustomMode NewCustomMode);

	bool IsHanging() const { return bIsHanging; }

	USR_DebugComponent* GetDebugComponent() const { return DebugComponent; }
protected:

	bool bIsHanging = false;
	FVector LedgeLocation;

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab")
	void CheckForLedgeGrab();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab")
	void ClimbUp();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void StartCrouch();

	UFUNCTION()
	void StopWallJump();

	UFUNCTION()
	void Slide();

	UFUNCTION()
	void StopSlide();

	void Dash(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	class USR_DashComponent* DashComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration")
	class USR_SlideComponent* SlideComponent;

	// assign a energy component to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	class USR_EnergyComponent* EnergyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	class USR_DebugComponent* DebugComponent;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
};
