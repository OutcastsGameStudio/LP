// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR_ClimbComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_ClimbComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_ClimbComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	
	virtual void EnterState(void* data) override;
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", meta = (ExposeOnSpawn = true))
	float ReachDistance = 20.f;

private:
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;
	UPROPERTY()
	ASR_Character* OwnerCharacter;
	UPROPERTY()
	USR_MotionController* MotionController;
	UPROPERTY()
	USR_ContextStateComponent* ContextStateComponent;

	FVector StartLocation = FVector::ZeroVector;

	void CheckForLedgeGrab();

	bool b_IsActive = false;

private:
	float LedgeGrabReachDistance = 70.0f;
	float LedgeGrabHeight = 150.0f;
	float ClimbUpSpeed = 20.0f;

	FVector LedgeLocation;

	int32 m_ClimbMotionId = -1;
};
