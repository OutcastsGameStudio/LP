#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SR_State.generated.h"


// This class does not need to be modified.
UINTERFACE()
class USR_State : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SR_API ISR_State
{
	GENERATED_BODY()
public:
	virtual void EnterState(void *data) = 0;
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) = 0;
	virtual bool LookAheadQuery() = 0;
	virtual void UpdateState() = 0;
	virtual FName GetStateName() const = 0;
	virtual int32 GetStatePriority() const = 0;
	virtual bool IsStateActive() const = 0;
};
