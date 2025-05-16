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
    virtual void EnterState(void* Data) override;
    
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;
    
    virtual bool LookAheadQuery() override;
    virtual void UpdateState() override;
    virtual FName GetStateName() const override;
    virtual int32 GetStatePriority() const override;
    virtual bool IsStateActive() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run", 
              meta = (ToolTip = "Acceleration of character when falling from wall run"))
    float WallRunFallingAcceleration = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run",
              meta = (ToolTip = "Maximum angle before stopping wall run"))
    float MaxAngleBeforeStop = 90.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run",
              meta = (ToolTip = "Wall run deceleration ratio"))
    float WallRunDecelerationRatio = 0.8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run", 
              meta = (ToolTip = "Maximum speed during wall run", ClampMin = "1.0", ClampMax= "10000.0"))
    float MaxWallRunClampedSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run", 
              meta = (ToolTip = "Minimum speed during wall run", ClampMin = "1.0", ClampMax= "10000.0"))
    float MinWallRunClampedSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run", 
              meta = (ToolTip = "Minimum speed required to trigger wall run", ClampMin = "1.0", ClampMax= "10000.0"))
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
    
    bool bIsMovingForward = false;
    
    UPROPERTY(EditDefaultsOnly, Category = "Wall Run", 
              meta = (ToolTip = "Maximum threshold for Z component to detect vertical surfaces"))
    float MaxZThreshold = 0.01;

    bool bIsStateActive = false;

    FVector WallNormal = FVector::ZeroVector;
    FVector WallRunDirection = FVector::ZeroVector;

    float WallRunFallingSpeed = 0;
    
    void UpdateState(float DeltaTime);
    
    UFUNCTION()
    void OnJumpButtonPressed();

    UFUNCTION()
    void OnJumpButtonReleased();

    // Debug
    void DebugLineTrace(FHitResult HitResult, bool bHit, FColor Color, FVector VectorStart, FVector VectorEnd);

private:
    UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", 
              meta = (ClampMin = "0.0", ClampMax = "45.0", 
              ToolTip = "Camera roll angle during wall run. Higher values tilt the camera more"))
    float WallRunCameraRollAngle = 15.0f;
    
    UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", 
              meta = (ClampMin = "0.0", ClampMax = "50.0",
              ToolTip = "Camera roll rotation speed during wall run"))
    float CameraRollSpeed = 20.0f;

    UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", 
              meta = (ClampMin = "0.0", ClampMax = "30.0",
              ToolTip = "Camera offset angle from the run direction"))
    float WallRunCameraOffset = 20.0f;
    
    float CurrentCameraRoll = 0.0f;

    float StartCameraRoll = 0.0f;

    bool bResettingCamera;

    float CameraResetTimer = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Wall Run | Camera", 
              meta = (ClampMin = "0.1", ClampMax = "5.0", 
              ToolTip = "Duration of camera transition back to normal after wall run"))
    float CameraResetDuration = 0.3f;

    FRotator LastCameraRotation;

    float MousePosXAtWallRunStart = 0.0f;
    float MousePosYAtWallRunStart = 0.0f;

    int32 WallRunSide = 0;

    void UpdateCameraRotation(float DeltaTime);
    bool HasDetectedPlayerMouseRotation(float DeltaTime);
    void ResetCameraRotation(float DeltaTime);

    int32 RootMotionId = -1;
};