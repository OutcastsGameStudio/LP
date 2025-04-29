// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "SR_State.h"
#include "Components/ActorComponent.h"
#include "SR_ContextStateComponent.generated.h"


class ASR_Character;

enum class MotionState
{
	NONE,
	WALL_RUN,
	WALL_JUMP,
	SLIDE,
	DASH,
	CLIMB,
	CROUCH,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_ContextStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_ContextStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	void TransitionState(MotionState NewStateName, bool bForced = false);

	void TransitionState(MotionState NewStateName, void* data, bool bForced = false);


	UFUNCTION(BlueprintCallable, Category = "Movement")
	FName GetCurrentStateName();

	MotionState GetCurrentMotionState() const
	{
		return m_CurrentMotionState;
	}
private:

	void RegisterStates();

	ISR_State* m_CurrentState = nullptr;

	std::map<MotionState, ISR_State*> m_States;

	FName m_CurrentStateName = "None";
	
	MotionState m_CurrentMotionState = MotionState::NONE;
	
	UPROPERTY()
	ASR_Character* m_Character = nullptr;

	void TransitionGuard(MotionState newState);

	void DebugState(MotionState newState);

	FName GetNameMyMotionState(MotionState state) const
	{
		switch (state)
		{
			case MotionState::NONE:
				return "None";
			case MotionState::WALL_RUN:
				return "WallRun";
			case MotionState::WALL_JUMP:
				return "WallJump";
			case MotionState::SLIDE:
				return "Slide";
			case MotionState::DASH:
				return "Dash";
			case MotionState::CLIMB:
				return "Climb";
			case MotionState::CROUCH:
				return "Crouch";
			default:
				return "Unknown";
		}
	}
};
