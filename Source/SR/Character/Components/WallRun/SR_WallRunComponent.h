// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Motion/SR_MotionController.h"
#include "SR_WallRunComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_WallRunComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_WallRunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	virtual void EnterState(void* data) override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", meta = (ToolTip = "Acceleration du personnage lorsqu'il tombe du wall run"))
	float WallRunFallingAcceleration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
	float MaxAngleBeforeStop = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
	float WallRunDecelerationRatio = 0.8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", 
	meta = (ToolTip = "Vitesse maximale pendant le wall run.", ClampMin = "1.0", ClampMax= "10000.0"))
	float MaxWallRunClampedSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", 
meta = (ToolTip = "Vitesse minimale pendant le wall run.", ClampMin = "1.0", ClampMax= "10000.0"))
	float MinWallRunClampedSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", 
meta = (ToolTip = "Vitesse minimale pour trigger le wall run.", ClampMin = "1.0", ClampMax= "10000.0"))
	float MinWallRunSpeed = 100.0f;
private:
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;
	UPROPERTY()
	ASR_Character* OwnerCharacter;
	UPROPERTY()
	USR_MotionController* MotionController;
	UPROPERTY()
	USR_ContextStateComponent* ContextStateComponent;

	UFUNCTION()
	void OnMoveForwardInputPressed();
	UFUNCTION()
	void OnMoveForwardInputReleased();

	bool DetectNextWall(FHitResult& Hit);

	bool CheckIfNotInCorner();

	void StopWallRun();

	bool m_IsMovingForward = false;
	float MAX_Z_THRE_HOLD = 0.01;

	bool bIsStateActive = false;

	FVector m_WallNormal = FVector::ZeroVector;
	FVector m_WallRunDirection = FVector::ZeroVector;

	float WallRunFallingSpeed = 0;
	
	void UpdateState(float deltaTime);
	
	UFUNCTION()
	void OnJumpButtonPressed();

	UFUNCTION()
	void OnJumpButtonReleased();

	// Debug
	void DebugLineTrace(FHitResult hitResult, bool hit, FColor color,FVector vectorStart, FVector vectorEnd);
};
