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
    ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
    MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();
    
    if (!CharacterMovement || !OwnerCharacter || !MotionController || !ContextStateComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_DashComponent::BeginPlay()"));
        return;
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
    ContextStateComponent->TransitionState(MotionState::DASH);
}

void USR_DashComponent::UpdateState()
{
    bool bIsInAir = !CharacterMovement->IsMovingOnGround();
    float AdjustedDashSpeed = bIsInAir ? DashSpeed : DashSpeed * GroundAirRatioForce;
    
    FRootMotionRequest Request;
    Request.MovementName = FName("Dash");
    Request.Strength = AdjustedDashSpeed;
    Request.Duration = 0.1f;
    Request.Direction = OwnerCharacter->GetActorForwardVector();
    Request.bIsAdditive = false;
    Request.Priority = RootMotionPriority::High;
    Request.bEnableGravity = false;
    
    if(bIsInAir)
    {
        Request.VelocityOnFinish = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
    } else
    {
        Request.VelocityOnFinish = ERootMotionFinishVelocityMode::SetVelocity;
        Request.SetVelocityOnFinish = OwnerCharacter->GetActorForwardVector() * (DashSpeed * 0.8f);
    }
    
    m_CurrentRootMotionID = MotionController->ApplyRootMotion(Request);

    // disable ground friction
    if (!bIsInAir)
    {
        bOriginalGroundFriction = CharacterMovement->GroundFriction;
        CharacterMovement->GroundFriction = 0.0f;
    }
}


void USR_DashComponent::EnterState(void* data)
{
    if (bIsStateActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("USR_DashComponent::EnterState() - State is already active"));
        return;
    }
    UpdateState();
    bIsStateActive = true;
}

void USR_DashComponent::LeaveState(int32 rootMotionId, bool bForced)
{
    if(rootMotionId != m_CurrentRootMotionID) return;


    if(!ContextStateComponent)
        UE_LOG(LogTemp, Error, TEXT("Failed to load ContextState in USR_DashComponent::LeaveState()"));

    // enable ground friction
    if (CharacterMovement)
    {
        CharacterMovement->GroundFriction = bOriginalGroundFriction;
    }

    bIsStateActive = false;
    ContextStateComponent->TransitionState(MotionState::NONE, bForced);
}


// used to check if the state can be activated 
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
