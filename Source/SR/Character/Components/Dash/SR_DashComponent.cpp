// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Motion/SR_MotionController.h"


USR_DashComponent::USR_DashComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USR_DashComponent::BeginPlay()
{
    Super::BeginPlay();
    // Get the character movement component and the owner character
    OwnerCharacter = Cast<ASR_Character>(GetOwner());
    CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();

    USR_MotionController* MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();
    if (!CharacterMovement || !OwnerCharacter || !MotionController)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_DashComponent::BeginPlay()"));
    }
   
    OwnerCharacter->OnDashInputPressed.AddDynamic(this, &USR_DashComponent::Dash);
    MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_DashComponent::LeaveState);
}

void USR_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // @TODO: implement cooldown logic
    // // cooldown update
    // if (!bCanDash)
    // {
    //     CurrentCooldownTime += DeltaTime;
    //     if (CurrentCooldownTime >= DashCooldown)
    //     {
    //         bCanDash = true;
    //         CurrentCooldownTime = 0.0f;
    //     }
    // }
}

void USR_DashComponent::Dash()
{
    USR_ContextStateComponent* ContextState = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
    ContextState->TransitionState(MotionState::DASH);
}

void USR_DashComponent::UpdateState()
{
    USR_MotionController* MotionController = OwnerCharacter->FindComponentByClass<USR_MotionController>();
    FRootMotionRequest Request;
    Request.MovementName = FName("Dash");
    Request.Strength = DashSpeed;
    Request.StrengthOverTime = StrengthOverTime;
    Request.Duration = DashDuration;
    Request.VelocityOnFinish = ERootMotionFinishVelocityMode::ClampVelocity;
    Request.SetVelocityOnFinish = FVector::ZeroVector;
    Request.ClampVelocityOnFinish = 0.0f;
    Request.bEnableGravity = false;
    Request.Direction = OwnerCharacter->GetActorForwardVector();
    Request.bIsAdditive = false;
    Request.Priority = RootMotionPriority::High;
    m_CurrentRootMotionID = MotionController->ApplyRootMotion(Request);
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

void USR_DashComponent::EnterState()
{
    if (bIsStateActive) return;
    UpdateState();
    bIsStateActive = true;
}

void USR_DashComponent::LeaveState(int32 rootMotionId, bool bForced)
{
    if(rootMotionId != m_CurrentRootMotionID) return;

    USR_ContextStateComponent* ContextState = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();

    if(!ContextState)
        UE_LOG(LogTemp, Error, TEXT("Failed to load ContextState in USR_DashComponent::LeaveState()"));

    bIsStateActive = false;
    ContextState->TransitionState(MotionState::NONE, bForced);
}

bool USR_DashComponent::LookAheadQuery()
{
    return !bIsStateActive;
}

FName USR_DashComponent::GetStateName() const
{
    return FName("Dash");
}

int32 USR_DashComponent::GetStatePriority() const
{
    return 0; // max priority
}

bool USR_DashComponent::IsStateActive() const
{
    return bIsStateActive;
}
