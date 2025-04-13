// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_ContextStateComponent.h"
#include "SR/Character/Components/Dash/SR_DashComponent.h"


// Sets default values for this component's properties
USR_ContextStateComponent::USR_ContextStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USR_ContextStateComponent::BeginPlay()
{
	Super::BeginPlay();
	RegisterStates();
	m_Character = Cast<ASR_Character>(GetOwner());
}


// Called every frame
void USR_ContextStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USR_ContextStateComponent::TransitionState(MotionState NewStateName, bool bForced)
{
	m_CurrentMotionState = NewStateName;
	if(NewStateName == MotionState::NONE)
	{
		// not ideal here, ideally the NONE state should be its own class
		m_CurrentState = m_States[MotionState::NONE];
		m_Character->SetCurrentState(NewStateName);
		return;
	}
	m_CurrentState = m_States[NewStateName]; // needed ?
	if(m_CurrentState == NULL)
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found"));	
		return;
	}
	m_CurrentState->EnterState(nullptr);
	m_Character->SetCurrentState(NewStateName);
}

void USR_ContextStateComponent::TransitionState(MotionState NewStateName, void* data, bool bForced)
{
	m_CurrentMotionState = NewStateName;
	if(NewStateName == MotionState::NONE)
	{
		
		m_CurrentState = m_States[MotionState::NONE];
		m_Character->SetCurrentState(NewStateName);
		return;
	}
	m_CurrentState = m_States[NewStateName]; // needed ?
	if(m_CurrentState == NULL)
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found"));	
		return;
	}
	m_CurrentState->EnterState(data);
	m_Character->SetCurrentState(NewStateName);
}


//@TODO: do know why it not getting updated when trying to display the name of the state from the outside
FName USR_ContextStateComponent::GetCurrentStateName()
{
	return m_CurrentStateName;
}

void USR_ContextStateComponent::RegisterStates()
{
	ASR_Character* owner = Cast<ASR_Character>(GetOwner());
	m_States[MotionState::NONE] = nullptr;
	m_States[MotionState::DASH] = owner->GetState(MotionState::DASH); // maybe use a loop here
	m_States[MotionState::WALL_RUN] = owner->GetState(MotionState::WALL_RUN);
	m_States[MotionState::WALL_JUMP] = owner->GetState(MotionState::WALL_JUMP);;
	m_States[MotionState::CLIMB] = owner->GetState(MotionState::CLIMB);;
	// m_States[MotionState::CLIMB] = new USR_ClimbState(this);


	//set NONE state as default state
	m_CurrentState = m_States[MotionState::NONE];
}

