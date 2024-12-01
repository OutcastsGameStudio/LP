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
	// Sets default values for this component's properties
	USR_DashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Dash in a direction in the world space where the character is facing or moving using the input value as a direction vector when he is on the ground
	// or dash in the direction of the camera when he is in the air and using the input value as a direction vector
	void Dash(FVector DashDirection);

	// dash timer
	FTimerHandle DashTimer;

	// EndDash function
	void EndDash();

	//get the character movement component
	UCharacterMovementComponent* CharacterMovement = nullptr;
};
