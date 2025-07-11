// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Motion/SR_MotionController.h"
#include "SR/Character/SR_Character.h"
#include "SR_DashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashEnded);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_DashComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	USR_DashComponent();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Dash();

	// Nouvel événement qui se déclenche au début du dash
	UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
	FOnDashStarted OnDashStarted;

	// Nouvel événement qui se déclenche à la fin du dash
	UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
	FOnDashEnded OnDashEnded;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector DashDirection;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings",
			  meta = (ToolTip = "Speed of dash when in air"))
	float DashSpeedInAir = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings",
			  meta = (ToolTip = "Speed of dash when on ground"))
	float DashSpeedOnGround = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings",
			  meta = (ToolTip = "Cooldown time between dashes in air"))
	float DashCooldownInAir = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings",
			  meta = (ToolTip = "Cooldown time between dashes on ground"))
	float DashCooldownOnGround = 0.5f;

	void ResetAllCooldowns();

public:
	virtual void EnterState(void *Data) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;

	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

private:
	UPROPERTY()
	UCharacterMovementComponent *CharacterMovement;

	UPROPERTY()
	ASR_Character *OwnerCharacter;

	UPROPERTY()
	USR_MotionController *MotionController;

	UPROPERTY()
	USR_ContextStateComponent *ContextStateComponent;

	int32 CurrentRootMotionID = 0;
	bool bIsStateActive = false;
	float OriginalGroundFriction = 0.0f;
	float CurrentCooldownTimeInAir = 0.0f;
	float CurrentCooldownTimeOnGround = 0.0f;
	bool bCanDashInAir = true;
	bool bCanDashOnGround = true;

	MotionState PreviousMotionState = MotionState::NONE;
	bool bPreviousGroundState = true;
};
