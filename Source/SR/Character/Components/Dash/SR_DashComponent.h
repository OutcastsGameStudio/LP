// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Motion/SR_MotionController.h"
#include "SR_DashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))


class SR_API USR_DashComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:    
	// Constructor
	USR_DashComponent();

	/**
	 * @description : Call when player dash input is triggered
	 * @name : Dash
	 * @param 
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Dash();

protected:
	// called when the game starts
	virtual void BeginPlay() override;

public:    
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
							   FActorComponentTickFunction* ThisTickFunction) override;

	// Dash Direction
	FVector DashDirection;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDuration = 0.2f;

public:
	virtual void EnterState(void* data) override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings", meta = (ExposeOnSpawn = true))
	UCurveFloat* StrengthOverTime = nullptr;
private:
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;
	// Reference to the owner character
	UPROPERTY()
	ASR_Character* OwnerCharacter;
	UPROPERTY()
	USR_MotionController* MotionController;
	UPROPERTY()
	USR_ContextStateComponent* ContextStateComponent;
	
	int32 m_CurrentRootMotionID = 0;
	bool bIsStateActive = false;
};
