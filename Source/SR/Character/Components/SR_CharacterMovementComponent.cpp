// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_CharacterMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "SR/Character/SR_Character.h"

// Sets default values for this component's properties
USR_CharacterMovementComponent::USR_CharacterMovementComponent() { PrimaryComponentTick.bCanEverTick = true; }
// Called when the game starts
void USR_CharacterMovementComponent::BeginPlay() { Super::BeginPlay(); }

// Called every frame
void USR_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
												   FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
