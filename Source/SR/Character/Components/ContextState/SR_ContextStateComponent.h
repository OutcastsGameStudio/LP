// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "SR_State.h"
#include "Components/ActorComponent.h"
#include "SR_ContextStateComponent.generated.h"


enum class MotionState
{
	NONE,
	WALL_RUN,
	SLIDE,
	DASH,
	CLIMB,
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
private:

	void RegisterStates();
	
	ISR_State* m_CurrentState = nullptr;

	std::map<MotionState, ISR_State*> m_States;
};
