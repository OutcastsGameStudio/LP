#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"
#include "SR_GrapplingHookComponent.generated.h"

class USR_MotionController;
class ASR_Character;
class UCapsuleComponent;
class ASR_GrapplePoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrapplingStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrapplingEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrapplePointDetected, ASR_GrapplePoint*, GrapplePoint);

USTRUCT(BlueprintType)
struct FGrappleData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Grapple")
    FVector GrapplePoint;

    UPROPERTY(BlueprintReadWrite, Category = "Grapple")
    float CableLength;

    UPROPERTY(BlueprintReadWrite, Category = "Grapple")
    ASR_GrapplePoint* TargetGrapplePoint;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_GrapplingHookComponent : public UActorComponent, public ISR_State
{
    GENERATED_BODY()

public:
    USR_GrapplingHookComponent();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void LaunchGrapple();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void ReleaseGrapple();

    UFUNCTION(BlueprintPure, Category = "Grapple")
    ASR_GrapplePoint* GetTargetedGrapplePoint() const { return CurrentTargetedPoint; }

    UFUNCTION(BlueprintPure, Category = "Grapple")
    bool HasValidGrappleTarget() const { return CurrentTargetedPoint != nullptr; }

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplingStarted OnGrapplingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplingEnded OnGrapplingEnded;

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplePointDetected OnGrapplePointDetected;

	UFUNCTION(blueprintCallable, Category = "Grappling")
	void OnGrappleInputPressed();

	UFUNCTION(BlueprintCallable, Category = "Grappling")
	void OnGrappleInputReleased();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                              FActorComponentTickFunction* ThisTickFunction) override;

    virtual void EnterState(void* Data) override;
    
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;
    
    virtual bool LookAheadQuery() override;
    virtual void UpdateState() override;
    virtual FName GetStateName() const override;
    virtual int32 GetStatePriority() const override;
    virtual bool IsStateActive() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings", 
              meta = (ToolTip = "Maximum distance for grappling hook to reach"))
    float MaxGrappleDistance = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Speed of initial pull towards grapple point"))
    float GrapplePullSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Force applied during swinging motion"))
    float SwingForce = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Multiplier for maintaining cable length constraint"))
    float CableConstraintStrength = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Cooldown time between grapple uses"))
    float GrappleCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Minimum velocity to maintain swinging state (only applies when button is released)"))
    float MinSwingVelocity = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Gravity multiplier during grappling"))
    float GrappleGravityScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Damping factor for swing motion"))
    float SwingDamping = 0.02f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Angle tolerance for grapple point detection (degrees)"))
    float GrappleDetectionAngle = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "If true, swing continues while button is held"))
    bool bContinuousSwingMode = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Show grapple points that are in range"))
    bool bHighlightGrapplePoints = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Visual",
              meta = (ToolTip = "Show debug line for grapple cable"))
    bool bShowDebugCable = true;

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
    UCapsuleComponent* CapsuleComponent;

    UPROPERTY()
    ASR_GrapplePoint* CurrentTargetedPoint;

    UPROPERTY()
    ASR_GrapplePoint* ActiveGrapplePoint;

    int32 CurrentRootMotionID = 0;
    bool bIsStateActive = false;
    bool bCanGrapple = true;
    bool bIsSwinging = false;
    bool bIsButtonHeld = false;
    float CurrentCooldownTime = 0.0f;

    FVector GrapplePoints;
    FVector InitialGrappleLocation;
    float CableLength = 0.0f;
    float OriginalGravityScale = 1.0f;
    
    

    ASR_GrapplePoint* FindBestGrapplePoint();
    void UpdateGrapplePointDetection();
    void StartGrapplePull();
    void ProcessSwing(float DeltaTime);
    FVector CalculateSwingForce(float DeltaTime);
    FVector CalculateCableConstraintForce();
    
    bool ShouldReleaseGrapple();
    void SetupCableVisuals();
    void UpdateCableVisuals();
    void HideCableVisuals();
    
    float GetDistanceToGrapplePoint() const;
    bool IsGrapplePointInView(ASR_GrapplePoint* GrapplePointActor) const;
};