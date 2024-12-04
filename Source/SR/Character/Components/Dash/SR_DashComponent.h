// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_DashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_DashComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	// Constructor
	USR_DashComponent();

	// Dash Function
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

protected:
	// Component of the character movement
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;

	// Reference to the owner character
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	// Properties of the dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashCooldown = 1.0f;

	

private:
	// Dash update
	void UpdateDash(float DeltaTime);

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
};
