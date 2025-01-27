// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USR_DashComponent::USR_DashComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USR_DashComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get the character movement component and the owner character
    CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
    OwnerCharacter = Cast<ACharacter>(GetOwner());

    if (!CharacterMovement || !OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("Dash Component: Failed to get Character or CharacterMovement"));
    }
}

void USR_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // dash update
    if (bIsDashing)
    {
        UpdateDash(DeltaTime);
    }

    // cooldown update
    if (!bCanDash)
    {
        CurrentCooldownTime += DeltaTime;
        if (CurrentCooldownTime >= DashCooldown)
        {
            bCanDash = true;
            CurrentCooldownTime = 0.0f;
        }
    }
}

void USR_DashComponent::Dash()
{   
    // check if the dash is available
    if (!bCanDash || !CharacterMovement || !OwnerCharacter) return;

    // Disable the dash
    bCanDash = false;
    bIsDashing = true;

    // Normalize the dash direction
    DashDirection.Normalize();

    // Stock the start location of the dash
    DashStartLocation = OwnerCharacter->GetActorLocation();

    // Reset the dash time
    CurrentDashTime = 0.0f;

    CurveValue = 0.0f;
    
    // Desactivate gravity
    CharacterMovement->GravityScale = 0.0f;
    CharacterMovement->BrakingDecelerationFalling = 0.0f;

    // Optional: Add a visual/sound effect at the start of the dash
    UE_LOG(LogTemp, Warning, TEXT("Dash Started!"));
}

void USR_DashComponent::UpdateDash(float DeltaTime)
{
    if (!bIsDashing || !OwnerCharacter) return;

    // Increment the dash time
    CurrentDashTime += DeltaTime;

    // Calculate the time progress (between 0 and 1)
    float TimeProgress = FMath::Clamp(CurrentDashTime / DashDuration, 0.0f, 1.0f);

    // Get the Y value from the curve based on time progress
    CurveValue = DashCurve ? DashCurve->GetFloatValue(TimeProgress) : TimeProgress;
    
    /**
    *   Calculate the new location
    *   
    *   Speed
    *   ^
    *   |      /‾‾‾‾‾‾‾
    *   |    /´
    *   |  /
    *   | /
    *   |/
    *   +--------------------> Time
    *   This is a curve of acceleration/deceleration which will give a better feeling to the dash
    *   exemple :
    *   at 25% of the time (alpha = 0.25) : 1.0f - FMath::Pow(1.0f - 0.75, 3) = 0.421875
    *   at 50% of the time (alpha = 0.50) : 1.0f - FMath::Pow(1.0f - 0.50, 3) = 0.875
    *   at 75% of the time (alpha = 0.75) : 1.0f - FMath::Pow(1.0f - 0.25, 3) = 0.984375
    */
    FVector NewLocation = DashStartLocation + (DashDirection * DashDistance * CurveValue);
    
    // Move the character
    OwnerCharacter->SetActorLocation(NewLocation, true);

    // check if the dash is finished
    if (CurveValue >= 1.0f)
    {
        EndDash();
    }
}

void USR_DashComponent::EndDash()
{
    if (!CharacterMovement) return;

    // reset the character movement
    CharacterMovement->GravityScale = 1.0f;
    CharacterMovement->BrakingDecelerationFalling = 960.0f; // Valeur par défaut

    CharacterMovement->SetMovementMode(MOVE_Walking);
    
    // Reset the flags
    bIsDashing = false;

    // Optional: Add a visual/sound effect at the end of the dash
    UE_LOG(LogTemp, Warning, TEXT("Dash Ended!"));
}