// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_TESTS

#include "Misc/Timespan.h"

#include "Containers/UnrealString.h"
#include "Tests/TestHarnessAdapter.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"

TEST_CASE_NAMED(FContextStateComponentTest, "USR_ContextStateComponent", "[StatePattern][Transitions][SmokeFilter]")
{
	SECTION("Should transition from DASH to WALL_RUN successfully")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();
		Context->InitializeStates();
		
		// Simulate dash state activation
		Context->TransitionToState(MotionState::DASH);
		CHECK(Context->GetCurrentState() == MotionState::DASH);
		
		// Test critical transition that was failing pre-refactoring
		bool transitionResult = Context->CanTransitionTo(MotionState::WALL_RUN);
		CHECK(transitionResult == true);
		
		Context->TransitionToState(MotionState::WALL_RUN);
		CHECK(Context->GetCurrentState() == MotionState::WALL_RUN);
	}
	
	SECTION("Should respect state priorities during conflicts")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();
		Context->InitializeStates();
		
		// Set current state to CLIMB (lower priority)
		Context->TransitionToState(MotionState::CLIMB);
		
		// DASH should override CLIMB due to higher priority
		Context->RequestStateTransition(MotionState::DASH);
		CHECK(Context->GetCurrentState() == MotionState::DASH);
	}
	
	SECTION("Should handle invalid transitions gracefully")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();
		Context->InitializeStates();
		
		// Test impossible transition (WALL_RUN to SLIDE without intermediate state)
		Context->TransitionToState(MotionState::WALL_RUN);
		bool invalidTransition = Context->CanTransitionTo(MotionState::SLIDE);
		CHECK(invalidTransition == false);
		
		// State should remain unchanged
		CHECK(Context->GetCurrentState() == MotionState::WALL_RUN);
	}
	
	SECTION("Should maintain state consistency during rapid transitions")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();
		Context->InitializeStates();
		
		// Simulate rapid player input sequence
		Context->TransitionToState(MotionState::DASH);
		Context->TransitionToState(MotionState::WALL_RUN);
		Context->TransitionToState(MotionState::WALL_JUMP);
		
		// Final state should be valid and consistent
		CHECK(Context->GetCurrentState() == MotionState::WALL_JUMP);
		CHECK(Context->IsInValidState() == true);
	}
}

#endif //WITH_TESTS
