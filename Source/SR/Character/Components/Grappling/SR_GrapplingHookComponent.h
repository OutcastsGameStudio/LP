#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"
#include "SR_GrapplingHookComponent.generated.h"

class ASR_Character;
class ASR_GrapplePoint;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrapplingStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrapplingEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrapplingLaunched);
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

USTRUCT()
struct FPendulumState
{
    GENERATED_BODY()

    // Angle du pendule (radians depuis la verticale)
    float Angle = 0.0f;
    
    // Vitesse angulaire (rad/s)
    float AngularVelocity = 0.0f;
    
    // Direction du plan de balancement (normalisée)
    FVector SwingPlaneNormal = FVector::RightVector;
    
    // Vecteur avant du plan de balancement
    FVector SwingForward = FVector::ForwardVector;
    
    // Position cartésienne relative au point d'ancrage
    FVector RelativePosition = FVector::ZeroVector;
    
    // Vitesse cartésienne
    FVector Velocity = FVector::ZeroVector;
    
    // Énergie totale du système
    float TotalEnergy = 0.0f;
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

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void LaunchFromGrapple();

    UFUNCTION(BlueprintPure, Category = "Grapple")
    ASR_GrapplePoint* GetTargetedGrapplePoint() const { return CurrentTargetedPoint; }

    UFUNCTION(BlueprintPure, Category = "Grapple")
    bool HasValidGrappleTarget() const { return CurrentTargetedPoint != nullptr; }

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplingStarted OnGrapplingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplingEnded OnGrapplingEnded;

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplingLaunched OnGrapplingLaunched;

    UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
    FOnGrapplePointDetected OnGrapplePointDetected;

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                              FActorComponentTickFunction* ThisTickFunction) override;

    virtual void EnterState(void* Data) override;
    virtual void LeaveState(int32 RootMotionId, bool bForced = false) override;
    virtual bool LookAheadQuery() override;
    virtual void UpdateState() override;
    virtual FName GetStateName() const override;
    virtual int32 GetStatePriority() const override;
    virtual bool IsStateActive() const override;

    // Paramètres du grappin
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings")
    float MaxGrappleDistance = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings")
    float GrapplePullSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings")
    float GrappleCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings")
    float GrappleDetectionAngle = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings")
    bool bContinuousSwingMode = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings")
    bool bHighlightGrapplePoints = true;

    // Nouveau : Distance de balancement (pourcentage de la longueur totale)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ClampMin = "0.3", ClampMax = "0.9", ToolTip = "Distance at which swinging starts (percentage of total cable length)"))
    float SwingDistanceRatio = 0.7f;

    // Nouveau : Type de pendule
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Use 2D pendulum (back and forth only) instead of 3D spherical pendulum"))
    bool bUse2DPendulum = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Allow slight steering of swing direction with left/right input"))
    bool bAllowSwingSteer = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling Settings",
              meta = (ToolTip = "Strength of swing steering"))
    float SwingSteerStrength = 50.0f;

    // Paramètres physiques
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Gravity acceleration in cm/s²"))
    float Gravity = 980.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Air resistance coefficient"))
    float AirDamping = 0.005f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Minimum swing speed to prevent getting stuck"))
    float MinimumSwingSpeed = 300.0f;

    // Nouveau : Paramètres d'impulsion
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Impulse strength when pressing forward/backward"))
    float ImpulseStrength = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Cooldown between impulses in seconds"))
    float ImpulseCooldown = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Maximum swing amplitude in degrees"))
    float MaxSwingAmplitude = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Launch boost multiplier when exiting grapple"))
    float LaunchBoostMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Additional upward velocity when launching"))
    float LaunchUpwardBoost = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Whether to add extra boost based on swing amplitude"))
    bool bUseAmplitudeBoost = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Maximum amplitude boost percentage (0-1)"))
    float MaxAmplitudeBoostPercent = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Apply air resistance to launch trajectory prediction"))
    bool bApplyAirResistanceToLaunch = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Air resistance coefficient for launch"))
    float LaunchAirResistance = 0.001f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Maximum angle from vertical (degrees)"))
    float MaxSwingAngle = 85.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings",
              meta = (ToolTip = "Integration timestep for physics"))
    float PhysicsTimestep = 0.016f;

    // Debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowDebugInfo = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowTrajectory = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowLaunchTrajectory = true;

private:
    UPROPERTY()
    ASR_Character* OwnerCharacter;

    UPROPERTY()
    USR_ContextStateComponent* ContextStateComponent;

    UPROPERTY()
    UCharacterMovementComponent* CharacterMovement;

    UPROPERTY()
    ASR_GrapplePoint* CurrentTargetedPoint;

    UPROPERTY()
    ASR_GrapplePoint* ActiveGrapplePoint;

    // État du système
    bool bIsStateActive = false;
    bool bCanGrapple = true;
    bool bIsSwinging = false;
    bool bIsButtonHeld = false;
    bool bIsPulling = false;
    float CurrentCooldownTime = 0.0f;

    // État physique
    FPendulumState PendulumState;
    FVector GrapplePoint;
    float CableLength = 0.0f;
    float SwingCableLength = 0.0f; // Longueur effective pour le balancement
    float PullProgress = 0.0f;
    
    // Nouveau : État de l'impulsion
    float LastImpulseTime = 0.0f;
    bool bCanApplyImpulse = true;
    float CurrentSwingAmplitude = 0.0f;
    
    // Input tracking
    bool bIsPressingForward = false;
    bool bIsPressingBackward = false;
    
    // Stockage de la vélocité avant grappin
    FVector PreGrappleVelocity;

    // Méthodes privées
    UFUNCTION()
    void OnGrappleInputPressed();

    UFUNCTION()
    void OnGrappleInputReleased();

    ASR_GrapplePoint* FindBestGrapplePoint();
    void UpdateGrapplePointDetection();
    bool IsGrapplePointInView(ASR_GrapplePoint* GrapplePointActor) const;

    // Phases du grappin
    void StartGrapplePull();
    void UpdatePullPhase(float DeltaTime);
    void StartSwingPhase();
    void UpdateSwingPhase(float DeltaTime);

    // Physique pure
    void InitializePendulumState();
    void UpdatePendulumPhysics(float DeltaTime);
    void ApplyImpulse(bool bForward);
    void IntegrateMotion(float DeltaTime);
    void EnforceCableConstraint();
    FVector CalculateNewPosition(float Angle, float Unused);

    // Utilitaires
    bool ShouldReleaseGrapple();
    void SetCharacterPosition(const FVector& NewPosition);
    void SetCharacterVelocity(const FVector& NewVelocity);
    void DrawDebugInfo();
    void DrawTrajectory();
    void DrawLaunchTrajectory();
    
    // Nouvelles méthodes pour gérer les inputs
    UFUNCTION()
    void OnMoveForwardInputPressed();
    
    UFUNCTION()
    void OnMoveForwardInputReleased();
    
    UFUNCTION()
    void OnMoveBackwardInputPressed();
    
    UFUNCTION()
    void OnMoveBackwardInputReleased();
};