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
	if (Speed >= maxSpeed)
	{
		// set the current speed to the max speed
		 return maxSpeed;
	}
	float newSpeed = Speed + acceleration * DeltaTime;
	// return the new speed
	return std::min(newSpeed, maxSpeed);
}

// function to increase the max speed of the character
void USR_AccelerationComponent::IncreaseMaxSpeed(float Value)
{
	// increase the max speed by the value
	maxSpeed += Value;
}

// function to reset the max speed of the character to the default value
void USR_AccelerationComponent::ResetMaxSpeed()
{
	// set the max speed to the default value
	maxSpeed = 2000.f;
}




