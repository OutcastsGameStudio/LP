// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SR_CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
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

public:
	ASR_Character();
	void Tick(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Grab")
	float LedgeGrabReachDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Grab")
	float LedgeGrabHeight = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Grab")
	float ClimbUpSpeed = 20.0f;

	/**
	 * @description : Set the custom movement mode of the character
	 * @param NewCustomMode 
	 */
	void SetCharacterMovementCustomMode(USR_CharacterMovementComponent::CustomMode NewCustomMode);
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
	
	UFUNCTION()
	void StopWallJump();

	UFUNCTION()
	void Slide();

	UFUNCTION()
	void StopSlide();

	//function to dash
	void Dash(const FInputActionValue& Value);

	// assign acceleration component to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration")
	class USR_AccelerationComponent* AccelerationComponent;

	//assign a dash component to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	class USR_DashComponent* DashComponent;

	// assign a slide component to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration")
	class USR_SlideComponent* SlideComponent;
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
};
