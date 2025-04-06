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
}


// Called every frame
void USR_ContextStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USR_ContextStateComponent::TransitionState(MotionState NewStateName, bool bForced)
{
	if(NewStateName == MotionState::NONE)
	{
		return;
	}
	m_CurrentState = m_States[NewStateName]; // needed ?
	m_CurrentState->EnterState();
}

void USR_ContextStateComponent::RegisterStates()
{
	ASR_Character* owner = Cast<ASR_Character>(GetOwner());
	m_States[MotionState::NONE] = nullptr;
	m_States[MotionState::DASH] = owner->GetState(MotionState::DASH); // maybe use a loop here
	// m_States[MotionState::SLIDE] = new USR_SlideState(this);
	// m_States[MotionState::DASH] = new USR_DashState(this);
	// m_States[MotionState::CLIMB] = new USR_ClimbState(this);


	//set NONE state as default state
	m_CurrentState = m_States[MotionState::NONE];
}

