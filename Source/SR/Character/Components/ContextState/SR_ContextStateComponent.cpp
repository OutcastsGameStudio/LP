// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_ContextStateComponent.h"
#include "SR/Character/Components/Dash/SR_DashComponent.h"

// Sets default values for this component's properties
USR_ContextStateComponent::USR_ContextStateComponent()
{
	// TickComponent ne fait rien d'autre que Super:: -> on désactive le tick.
	// Les transitions d'état sont event-driven (inputs / OnHit / OnRootMotionCompleted).
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void USR_ContextStateComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<ASR_Character>(GetOwner());
	RegisterStates();
}

// Called every frame
void USR_ContextStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											  FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USR_ContextStateComponent::TransitionState(MotionState NewStateName, bool bForced)
{
	// DebugState(NewStateName);
	TransitionGuard(NewStateName, bForced);
	CurrentMotionState = NewStateName;
	if (NewStateName == MotionState::NONE)
	{
		// not ideal here, ideally the NONE state should be its own class
		CurrentState = States[MotionState::NONE];
		return;
	}
	CurrentState = States[NewStateName];
	if (CurrentState == nullptr)
	{
		return;
	}
	CurrentState->EnterState(nullptr);
}

void USR_ContextStateComponent::TransitionState(MotionState NewStateName, void *data, bool bForced)
{
	// DebugState(NewStateName);
	TransitionGuard(NewStateName, bForced);
	CurrentMotionState = NewStateName;
	if (NewStateName == MotionState::NONE)
	{
		CurrentState = States[MotionState::NONE];
		return;
	}
	CurrentState = States[NewStateName];
	if (CurrentState == nullptr)
	{
		return;
	}
	CurrentState->EnterState(data);
}

//@TODO: do know why it not getting updated when trying to display the name of
// the state from the outside
FName USR_ContextStateComponent::GetCurrentStateName()
{
	if (CurrentState == nullptr)
	{
		return "None";
	}
	return CurrentState->GetStateName();
}

void USR_ContextStateComponent::RegisterStates()
{
	States[MotionState::NONE] = nullptr;
	States[MotionState::DASH] = Character->GetState(MotionState::DASH);
	States[MotionState::WALL_RUN] = Character->GetState(MotionState::WALL_RUN);
	States[MotionState::WALL_JUMP] = Character->GetState(MotionState::WALL_JUMP);
	States[MotionState::CLIMB] = Character->GetState(MotionState::CLIMB);
	States[MotionState::SLIDE] = Character->GetState(MotionState::SLIDE);

	// set NONE state as default state
	CurrentState = States[MotionState::NONE];
}

void USR_ContextStateComponent::TransitionGuard(MotionState newState, bool bForced)
{
	if (CurrentState == nullptr)
	{
		return;
	}

	if (CurrentState->IsStateActive())
	{
		CurrentState->LeaveState(0, bForced);
	}
}

void USR_ContextStateComponent::DebugState(MotionState newState)
{
	auto currentStateName = GetNameFromMotionState(CurrentMotionState);
	GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Purple,
									 TEXT("Current State: ") + currentStateName.ToString() +
										 " New State: " + GetNameFromMotionState(newState).ToString());
}
