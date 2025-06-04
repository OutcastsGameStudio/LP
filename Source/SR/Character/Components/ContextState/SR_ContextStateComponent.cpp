// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_ContextStateComponent.h"
#include "SR/Character/Components/Dash/SR_DashComponent.h"

// Sets default values for this component's properties
USR_ContextStateComponent::USR_ContextStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked
	// every frame.  You can turn these features off to improve performance if you
	// don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
		Character->SetCurrentState(NewStateName);
		return;
	}
	CurrentState = States[NewStateName];
	if (CurrentState == nullptr)
	{
		return;
	}
	CurrentState->EnterState(nullptr);
	Character->SetCurrentState(NewStateName);
}

void USR_ContextStateComponent::TransitionState(MotionState NewStateName, void *data, bool bForced)
{
	// DebugState(NewStateName);
	TransitionGuard(NewStateName, bForced);
	CurrentMotionState = NewStateName;
	if (NewStateName == MotionState::NONE)
	{
		CurrentState = States[MotionState::NONE];
		Character->SetCurrentState(NewStateName);
		return;
	}
	CurrentState = States[NewStateName];
	if (CurrentState == nullptr)
	{
		return;
	}
	CurrentState->EnterState(data);
	Character->SetCurrentState(NewStateName);
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
	States[MotionState::GRAPPLING_HOOK] = Character->GetState(MotionState::GRAPPLING_HOOK);

	// set NONE state as default state
	CurrentState = States[MotionState::NONE];
}

void USR_ContextStateComponent::TransitionGuard(MotionState newState, bool bForced = false)
{
	if (CurrentState == nullptr)
	{
		return;
	}

	/*if (CurrentState->IsStateActive())
	{
		CurrentState->LeaveState(0, bForced);
	}*/
}

void USR_ContextStateComponent::DebugState(MotionState newState)
{
	auto currentStateName = GetNameFromMotionState(CurrentMotionState);
	GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Purple,
									 TEXT("Current State: ") + currentStateName.ToString() +
										 " New State: " + GetNameFromMotionState(newState).ToString());
}
