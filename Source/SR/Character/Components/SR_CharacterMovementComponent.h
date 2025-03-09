// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_CharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USR_CharacterMovementComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


/**
 * @description: Wall Run Props
 */
private:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun")
	float WallJumpSpeed = 1000.f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", meta = (ToolTip = "Angle maximum a partir duquel on peut faire un wall run"))
	float MaxAngleWallRun = 60.f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",
	// meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
	float MaxAngleBeforeStop = 120.f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", meta = (ToolTip = "Acceleration du personnage lorsqu'il tombe du wall run"))
	float WallRunFallingAcceleration = 5.f;


public:
	float GetWallJumpSpeed();

	void SetWallJumpSpeed(float NewWallJumpSpeed);

	float GetMaxAngleWallRun();

	void SetMaxAngleWallRun(float NewMaxAngleWallRun);

	float GetMaxAngleBeforeStop();

	void SetMaxAngleBeforeStop(float NewMaxAngleBeforeStop);

	float GetWallRunFallingAcceleration();

	void SetWallRunFallingAcceleration(float NewWallRunFallingAcceleration);


	/**
	* @description: Stop the wall jump when the character is already wall running 
	*/

	void PhysWallRun(float deltaTime, int32 Iterations);
	void StopWallJump();
	bool IsWallRunning()
	{
		return m_bIsWallRunning;
	}


	void PhysFalling(float deltaTime, int32 Iterations) override;
private:
	bool DetectNextWall(FHitResult& Hit);
	bool CanWallRun();
	bool m_bIsWallRunning = false;
	FVector m_WallRunDirection;
	FVector m_WallNormal;
	float WallRunFallingSpeed = 0.f;


	float MAX_Z_THRE_HOLD = 0.01;
	float MAX_VELOCITY_Z_THRE_HOLD = 50.f; // little fix for now to avoid wall only when chartcer is falling, maybe use angle between camera lookAt and wall normal
	
/**
 *
 */

public:
	enum CustomMode
	{
		CUSTOM_None = 0,
		CUSTOM_WallRun = 1,
		CUSTOM_DASH = 2,
	};
	void SetCustomMode(int32 NewCustomMode)
	{
		m_CustomMovementMode = (CustomMode)NewCustomMode;
	}
private:
	CustomMode m_CustomMovementMode;

private:
	//** Debug *//
private:
	void DebugLineTrace(FHitResult hitResult, bool hit, FColor color,FVector vectorStart, FVector vectorEnd);
private:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    virtual void PhysCustom(float deltaTime, int32 Iterations) override;
};
