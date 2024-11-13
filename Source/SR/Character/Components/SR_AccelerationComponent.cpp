// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_AccelerationComponent.h"


// Sets default values for this component's properties
USR_AccelerationComponent::USR_AccelerationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void USR_AccelerationComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void USR_AccelerationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// function to accelerate the character over time which will return the new speed until it reaches the max speed
float USR_AccelerationComponent::Accelerate(float Speed, float DeltaTime)
{
	// if the current speed is less than the max speed
	if (CurrentSpeed < MaxSpeed)
	{
		// increase the current speed by the acceleration value
		CurrentSpeed += Acceleration * DeltaTime;
		// if the current speed is greater than the max speed
		if (CurrentSpeed > MaxSpeed)
		{
			// set the current speed to the max speed
			CurrentSpeed = MaxSpeed;
		}
	}
	// return the new speed
	return CurrentSpeed;
}

// function to decelerate the character over time until it reaches 0
float USR_AccelerationComponent::Decelerate(float Speed, float DeltaTime)
{
	// if the current speed is greater than 0
	if (CurrentSpeed > 0)
	{
		// decrease the current speed by the deceleration value
		CurrentSpeed -= Deceleration * DeltaTime;
		// if the current speed is less than 0
		if (CurrentSpeed < 0)
		{
			// set the current speed to 0
			CurrentSpeed = 0;
		}
	}
	// return the new speed
	return CurrentSpeed;
}

// function to increase the max speed of the character
void USR_AccelerationComponent::IncreaseMaxSpeed(float Value)
{
	// increase the max speed by the value
	MaxSpeed += Value;
}

// function to reset the max speed of the character to the default value
void USR_AccelerationComponent::ResetMaxSpeed()
{
	// set the max speed to the default value
	MaxSpeed = 1000.f;
}

// function to get the current speed of the character
float USR_AccelerationComponent::GetCurrentSpeed()
{
	// return the current speed
	return CurrentSpeed;
}

// function to set the current speed of the character
void USR_AccelerationComponent::SetCurrentSpeed(float Value)
{
	// set the current speed to the value
	CurrentSpeed = Value;
}



