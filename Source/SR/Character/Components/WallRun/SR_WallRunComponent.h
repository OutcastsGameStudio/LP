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
	UPROPERTY()
	APlayerController* PlayerController;

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

private:
	UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", meta = (ClampMin = "0.0", ClampMax = "45.0"), meta = (ToolTip = "Angle de la caméra pendant le wall run. Plus la valeur est élevée, plus la caméra est inclinée vers le cote."))
	float m_WallRunCameraRollAngle = 15.0f;
    
	UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", meta = (ClampMin = "0.0", ClampMax = "50.0"),meta = (ToolTip = "Vitesse de rotation de la caméra pendant le wall run. Plus la valeur est élevée, plus la caméra tourne vite."))
	float m_CameraRollSpeed = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", meta = (ClampMin = "0.0", ClampMax = "30.0"),meta = (ToolTip = "Offset de l'angle de la caméra par rapport a la direction de la course. Plus la valeur est élevée, plus la caméra est éloignée de la direction de la course."))
	float m_WallRunCameraOffset = 20.0f;
    
	float m_CurrentCameraRoll = 0.0f;

	float m_StartCameraRoll = 0.0f;

	bool bResettingCamera;

	float m_CameraResetTimer = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", meta = (ClampMin = "0.1", ClampMax = "5.0"), meta = (ToolTip = "Durée de la transition de la caméra vers la position de départ après le wall run. Plus la valeur est élevée, plus la transition est lente."))
	float m_CameraResetDuration = 0.3f;

	FRotator m_LastCameraRotation;

	float m_MousePosXAtWallRunStart = 0.0f;
	float m_MousePosYAtWallRunStart = 0.0f;

	int32 m_WallRunSide = 0;

	void UpdateCameraRotation(float DeltaTime);
	bool HasDetectedPlayerMouseRotation(float DeltaTime);
	void ResetCameraRotation(float DeltaTime);

	int m_rootMotionId = -1;
};
