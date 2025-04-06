// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR_DashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))


class SR_API USR_DashComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:    
	// Constructor
	USR_DashComponent();

	/**
	 * @description : Call when player dash input is triggered
	 * @name : Dash
	 * @param 
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Dash();

protected:
	// called when the game starts
	virtual void BeginPlay() override;

public:    
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
							   FActorComponentTickFunction* ThisTickFunction) override;

	// Dash Direction
	FVector DashDirection;

	float CharacterGravityScale;
	float CharacterBrakingDecelerationFalling;
	
	

protected:
	// Component of the character movement
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;

	// Reference to the owner character
	UPROPERTY()
	ASR_Character* OwnerCharacter;

	// Properties of the dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashCooldown = 1.0f;

	// UCurveFloat to get the y value of the curve
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	UCurveFloat* DashCurve;

	//set dash curve float value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float CurveValue = 0.0f;

private:
	// End of the dash
	void EndDash();

	// Start location of the dash
	FVector DashStartLocation;

	// Dash Current Time
	float CurrentDashTime;

	// Dash Current Cooldown Time
	float CurrentCooldownTime;

	// Dash State
	bool bIsDashing = false;
	bool bCanDash = true;


public:
	virtual void EnterState() override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings", meta = (ExposeOnSpawn = true))
	UCurveFloat* StrengthOverTime = nullptr;
private:
	int32 m_CurrentRootMotionID = 0;
	bool bIsStateActive = false;
};
