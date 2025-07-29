// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "SR_State.h"

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
	UNKNOWN
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

	bool IsValidState(MotionState State);

	void ClearStates();

	int GetStatesCount() const
	{
		return States.Num();
	}

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

	void TransitionState(MotionState NewStateName, bool bForced = true);

	void TransitionState(MotionState NewStateName, void *Data, bool bForced = true);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FName GetCurrentStateName();

	MotionState GetCurrentMotionState() const { return CurrentMotionState; }

	TMap<MotionState, ISR_State *> States = {{MotionState::NONE, nullptr},		{MotionState::WALL_RUN, nullptr},
											 {MotionState::WALL_JUMP, nullptr}, {MotionState::SLIDE, nullptr},
											 {MotionState::DASH, nullptr},		{MotionState::CLIMB, nullptr}};
private:
	void RegisterStates();

	ISR_State *CurrentState = nullptr;

	UPROPERTY()
	ASR_Character *Character = nullptr;


	FName CurrentStateName = "None";

	MotionState CurrentMotionState = MotionState::NONE;

	UPROPERTY()
	ASR_Character *OwnerCharacter = nullptr;

	void TransitionGuard(MotionState NewState, bool bForced);

	void DebugState(MotionState NewState);

	FName GetNameFromMotionState(MotionState State) const
	{
		switch (State)
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
		default:
			return "Unknown";
		}
	}
};
