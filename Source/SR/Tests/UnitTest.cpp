// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_TESTS

#include "Misc/Timespan.h"

#include "Containers/UnrealString.h"
#include "Tests/TestHarnessAdapter.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"

TEST_CASE_NAMED(FContextStateComponentTest, "USR_ContextStateComponent", "[Core][Time][SmokeFilter]")
{
	SECTION("Should pass if with valid states")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();

		// Test state validation
		CHECK(Context->IsValidState(MotionState::NONE) == true);
		CHECK(Context->IsValidState(MotionState::DASH) == true);
		CHECK(Context->IsValidState(MotionState::WALL_RUN) == true);
	}

	SECTION("Should not pass with invalid states")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();

		// Test state validation
		CHECK(Context->IsValidState(MotionState::UNKNOWN) == false);
	}
	
	SECTION("Should pass with correct number of states")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();

		// Test state validation
		CHECK(Context->GetStatesCount() == 6);
	}

	SECTION("Should pass with correct number of states when states are cleared")
	{
		USR_ContextStateComponent* Context = NewObject<USR_ContextStateComponent>();
		Context->ClearStates();
		// Test state validation
		CHECK(Context->GetStatesCount() == 0);
	}
}

#endif //WITH_TESTS
