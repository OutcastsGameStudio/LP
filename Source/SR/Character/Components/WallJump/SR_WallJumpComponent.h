// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/SR_Character.h"
#include "SR_WallJumpComponent.generated.h"

class USR_MotionController;

USTRUCT(BlueprintType)
struct FWallJumpData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Wall Jump")
	FVector WallRunDirection;

	UPROPERTY(BlueprintReadWrite, Category = "Wall Jump")
	FVector WallNormal;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_WallJumpComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_WallJumpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

	virtual void EnterState(void *Data) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;

	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump",
			  meta = (ExposeOnSpawn = true, ToolTip = "Curve that determines wall jump strength over time"))
	UCurveFloat *WallJumpStrengthCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump",
			  meta = (ToolTip = "Force applied when wall jumping"))
	float WallJumpForce = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump",
			  meta = (ToolTip = "Maximum duration of wall jump in seconds"))
	float Duration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump", meta = (ToolTip = "Speed of wall jump"))
	float WallJumpSpeed = 10.f;

	UPROPERTY(
		EditAnywhere, BlueprintReadWrite, Category = "Wall Jump",
		meta =
			(ToolTip =
				 "Multiplier for forward component when jumping from wall run. Higher values = jump farther in run direction. Default: 1.0"))
	float WallRunDirectionRatio = 1.0f;

	UPROPERTY(
		EditAnywhere, BlueprintReadWrite, Category = "Wall Jump",
		meta =
			(ToolTip =
				 "Multiplier for perpendicular component when jumping from wall run. Higher values = jump farther from wall. Default: 1.0"))
	float WallRunNormalRatio = 1.0f;

	UPROPERTY(
		EditAnywhere, BlueprintReadWrite, Category = "Wall Jump",
		meta =
			(ToolTip =
				 "Multiplier for vertical component when jumping from wall run. Higher values = jump higher. Default: 1.0"))
	float WallRunUpRatio = 1.0f;

private:
	UPROPERTY()
	UCharacterMovementComponent *CharacterMovement;

	UPROPERTY()
	ASR_Character *OwnerCharacter;

	UPROPERTY()
	USR_MotionController *MotionController;

	UPROPERTY()
	USR_ContextStateComponent *ContextStateComponent;

	UFUNCTION()
	void OnJumpButtonPressed();

	UFUNCTION()
	void OnJumpButtonReleased();

	FVector WallRunDirection;

	int32 WallRunMainMotionId = 0;

	FVector WallNormal;

	bool bIsActive = false;
};
