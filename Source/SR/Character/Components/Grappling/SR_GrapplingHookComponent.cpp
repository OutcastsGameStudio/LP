#include "SR_GrapplingHookComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Motion/SR_MotionController.h"
#include "SR/GameplayObjects/SR_GrapplePoint.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USR_GrapplingHookComponent::USR_GrapplingHookComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USR_GrapplingHookComponent::BeginPlay()
{
    Super::BeginPlay();
    
    OwnerCharacter = Cast<ASR_Character>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("USR_GrapplingHookComponent: Owner is not ASR_Character"));
        return;
    }
    
    CharacterMovement = OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>();
    ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();
    MotionController = GetOwner()->FindComponentByClass<USR_MotionController>();
    CapsuleComponent = OwnerCharacter->FindComponentByClass<UCapsuleComponent>();

    if (!CharacterMovement || !MotionController || !ContextStateComponent || !CapsuleComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_GrapplingHookComponent::BeginPlay()"));
        return;
    }

    OriginalGravityScale = CharacterMovement->GravityScale;
    
    OwnerCharacter->OnGrappleInputPressed.AddDynamic(this, &USR_GrapplingHookComponent::OnGrappleInputPressed);
    OwnerCharacter->OnGrappleInputReleased.AddDynamic(this, &USR_GrapplingHookComponent::OnGrappleInputReleased);
    
    MotionController->OnRootMotionCompleted.AddDynamic(this, &USR_GrapplingHookComponent::LeaveState);
}

void USR_GrapplingHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
                                              FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!bCanGrapple)
    {
        CurrentCooldownTime += DeltaTime;
        if (CurrentCooldownTime >= GrappleCooldown)
        {
            bCanGrapple = true;
            CurrentCooldownTime = 0.0f;
        }
    }
    
    if (!bIsStateActive)
    {
        UpdateGrapplePointDetection();
    }
    
    if (bIsStateActive && bIsSwinging)
    {
        ProcessSwing(DeltaTime);
        UpdateCableVisuals();
        
        if (ShouldReleaseGrapple())
        {
            ReleaseGrapple();
        }
    }
}

void USR_GrapplingHookComponent::UpdateGrapplePointDetection()
{
    ASR_GrapplePoint* NewTarget = FindBestGrapplePoint();
    
    if (NewTarget != CurrentTargetedPoint)
    {
        if (CurrentTargetedPoint && bHighlightGrapplePoints)
        {
            CurrentTargetedPoint->SetHighlighted(false);
        }
        
        CurrentTargetedPoint = NewTarget;
        
        if (CurrentTargetedPoint)
        {
            if (bHighlightGrapplePoints)
            {
                CurrentTargetedPoint->SetHighlighted(true);
            }
            OnGrapplePointDetected.Broadcast(CurrentTargetedPoint);
        }
    }
}

ASR_GrapplePoint* USR_GrapplingHookComponent::FindBestGrapplePoint()
{
    TArray<AActor*> FoundGrapplePoints;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASR_GrapplePoint::StaticClass(), FoundGrapplePoints);
    
    ASR_GrapplePoint* BestGrapplePoint = nullptr;
    float BestScore = -1.0f;
    
    UCameraComponent* FollowCamera = OwnerCharacter->GetFollowCamera();
    if (!FollowCamera)
    {
        return nullptr;
    }
    
    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector CameraForward = FollowCamera->GetForwardVector();
    
    for (AActor* Actor : FoundGrapplePoints)
    {
        ASR_GrapplePoint* GrapplePoint = Cast<ASR_GrapplePoint>(Actor);
        if (!GrapplePoint || !GrapplePoint->IsActive())
        {
            continue;
        }
        
        FVector ToGrapplePoint = GrapplePoint->GetActorLocation() - CameraLocation;
        float Distance = ToGrapplePoint.Size();
        
        if (Distance > MaxGrappleDistance)
        {
            continue;
        }
        
        ToGrapplePoint.Normalize();
        float DotProduct = FVector::DotProduct(CameraForward, ToGrapplePoint);
        float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
        
        if (Angle > GrappleDetectionAngle)
        {
            continue;
        }
        
        if (!GrapplePoint->CanBeGrappled(OwnerCharacter->GetActorLocation()))
        {
            continue;
        }
        
        if (!IsGrapplePointInView(GrapplePoint))
        {
            continue;
        }
        
        float Score = DotProduct * (1.0f - (Distance / MaxGrappleDistance));
        
        if (Score > BestScore)
        {
            BestScore = Score;
            BestGrapplePoint = GrapplePoint;
        }
    }
    
    return BestGrapplePoint;
}

bool USR_GrapplingHookComponent::IsGrapplePointInView(ASR_GrapplePoint* GrapplePointActor) const
{
    if (!GrapplePointActor || !OwnerCharacter)
    {
        return false;
    }
    
    UCameraComponent* FollowCamera = OwnerCharacter->GetFollowCamera();
    if (!FollowCamera)
    {
        return false;
    }
    
    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector TargetLocation = GrapplePointActor->GetGrappleLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);
    QueryParams.AddIgnoredActor(GrapplePointActor);
    QueryParams.bTraceComplex = false;
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        CameraLocation,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );
    
    return !bHit;
}

void USR_GrapplingHookComponent::OnGrappleInputPressed()
{
    bIsButtonHeld = true;
    
    if (bIsStateActive)
    {
        if (!bContinuousSwingMode)
        {
            ReleaseGrapple();
        }
    }
    else
    {
        LaunchGrapple();
    }
}

void USR_GrapplingHookComponent::OnGrappleInputReleased()
{
    bIsButtonHeld = false;
    
    if (bIsStateActive && bContinuousSwingMode)
    {
        ReleaseGrapple();
    }
}

void USR_GrapplingHookComponent::LaunchGrapple()
{
    if (!bCanGrapple || bIsStateActive || !CurrentTargetedPoint)
    {
        return;
    }
    
    ActiveGrapplePoint = CurrentTargetedPoint;
    GrapplePoints = ActiveGrapplePoint->GetGrappleLocation();
    
    FGrappleData GrappleData;
    GrappleData.GrapplePoint = GrapplePoints;
    GrappleData.CableLength = FVector::Distance(OwnerCharacter->GetActorLocation(), GrapplePoints);
    GrappleData.TargetGrapplePoint = ActiveGrapplePoint;
    
    ContextStateComponent->TransitionState(MotionState::GRAPPLING_HOOK, &GrappleData);
}

void USR_GrapplingHookComponent::ReleaseGrapple()
{
    if (bIsStateActive)
    {
        LeaveState(CurrentRootMotionID, true);
    }
}

void USR_GrapplingHookComponent::EnterState(void* Data)
{
    if (bIsStateActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("USR_GrapplingHookComponent::EnterState() - State is already active"));
        return;
    }
    
    FGrappleData* GrappleData = static_cast<FGrappleData*>(Data);
    if (GrappleData)
    {
        GrapplePoints = GrappleData->GrapplePoint;
        CableLength = GrappleData->CableLength;
        ActiveGrapplePoint = GrappleData->TargetGrapplePoint;
    }
    
    bIsStateActive = true;
    bCanGrapple = false;
    bIsSwinging = false;
    CurrentCooldownTime = 0.0f;
    
    InitialGrappleLocation = OwnerCharacter->GetActorLocation();
    
    CharacterMovement->GravityScale = GrappleGravityScale;
    
    SetupCableVisuals();
    
    if (ActiveGrapplePoint)
    {
        ActiveGrapplePoint->OnGrappleAttached(OwnerCharacter);
    }
    
    StartGrapplePull();
    
    OnGrapplingStarted.Broadcast();
}

void USR_GrapplingHookComponent::StartGrapplePull()
{
    FVector Direction = (GrapplePoints - OwnerCharacter->GetActorLocation()).GetSafeNormal();
    
    FRootMotionRequest Request;
    Request.MovementName = FName("GrappleInitialPull");
    Request.Direction = Direction;
    Request.Strength = GrapplePullSpeed;
    Request.Duration = 0.3f;
    Request.bIsAdditive = false;
    Request.Priority = ERootMotionPriority::High;
    Request.VelocityOnFinish = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
    Request.bEnableGravity = true;
    
    CurrentRootMotionID = MotionController->ApplyRootMotion(Request);
    
    FTimerHandle TransitionTimer;
    GetWorld()->GetTimerManager().SetTimer(TransitionTimer, [this]()
    {
        bIsSwinging = true;
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }, 0.3f, false);
}

void USR_GrapplingHookComponent::ProcessSwing(float DeltaTime)
{
    if (!CharacterMovement)
    {
        return;
    }
    
    FVector SwingForceVector = CalculateSwingForce(DeltaTime);
    FVector ConstraintForce = CalculateCableConstraintForce();
    
    FVector TotalForce = SwingForceVector + ConstraintForce;
    
    FVector CurrentVelocity = CharacterMovement->Velocity;
    FVector DampingForce = -CurrentVelocity * SwingDamping;
    TotalForce += DampingForce;
    
    if (!TotalForce.IsNearlyZero())
    {
        FRootMotionRequest SwingRequest;
        SwingRequest.MovementName = FName("GrappleSwing");
        SwingRequest.Direction = TotalForce.GetSafeNormal();
        SwingRequest.Strength = TotalForce.Size();
        SwingRequest.Duration = DeltaTime;
        SwingRequest.bIsAdditive = true;
        SwingRequest.Priority = ERootMotionPriority::Medium;
        SwingRequest.bEnableGravity = true;
        
        MotionController->ApplyRootMotion(SwingRequest);
    }
}

FVector USR_GrapplingHookComponent::CalculateSwingForce(float DeltaTime)
{
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    FVector InputVector = FVector::ZeroVector;
    
    if (CharacterMovement)
    {
        FVector LastInput = CharacterMovement->GetLastInputVector();
        if (!LastInput.IsNearlyZero())
        {
            LastInput.Z = 0;
            LastInput.Normalize();
            
            FVector CableDirection = (GrapplePoints - CurrentLocation).GetSafeNormal();
            FVector HorizontalCableDir = FVector(CableDirection.X, CableDirection.Y, 0).GetSafeNormal();
            FVector SwingDirection = FVector::CrossProduct(FVector::UpVector, HorizontalCableDir);
            
            float InputDot = FVector::DotProduct(LastInput, SwingDirection);
            InputVector = SwingDirection * InputDot * SwingForce;
        }
    }
    
    return InputVector;
}

FVector USR_GrapplingHookComponent::CalculateCableConstraintForce()
{
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    FVector DirectionToAnchor = (GrapplePoints - CurrentLocation);
    float CurrentDistance = DirectionToAnchor.Size();
    DirectionToAnchor.Normalize();
    
    float DistanceDelta = CurrentDistance - CableLength;
    
    if (DistanceDelta > 0)
    {
        return DirectionToAnchor * DistanceDelta * CableConstraintStrength;
    }
    
    return FVector::ZeroVector;
}

bool USR_GrapplingHookComponent::ShouldReleaseGrapple()
{
    if (bContinuousSwingMode && bIsButtonHeld)
    {
        return false;
    }
    
    if (CharacterMovement->Velocity.Size() < MinSwingVelocity)
    {
        return true;
    }
    
    if (CharacterMovement->IsMovingOnGround())
    {
        return true;
    }
    
    if (GetDistanceToGrapplePoint() < 10.0f)
    {
        return true;
    }
    
    return false;
}

void USR_GrapplingHookComponent::LeaveState(int32 RootMotionId, bool bForced)
{
    if (!bForced && RootMotionId != CurrentRootMotionID)
    {
        return;
    }
    
    bIsStateActive = false;
    bIsSwinging = false;
    bIsButtonHeld = false;
    
    CharacterMovement->GravityScale = OriginalGravityScale;
    
    if (CharacterMovement->IsMovingOnGround())
    {
        CharacterMovement->SetMovementMode(MOVE_Walking);
    }
    else
    {
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }
    
    HideCableVisuals();
    
    if (ActiveGrapplePoint)
    {
        ActiveGrapplePoint->OnGrappleDetached();
        ActiveGrapplePoint = nullptr;
    }
    
    bCanGrapple = false;
    CurrentCooldownTime = 0.0f;
    
    ContextStateComponent->TransitionState(MotionState::NONE, bForced);
    
    OnGrapplingEnded.Broadcast();
}

bool USR_GrapplingHookComponent::LookAheadQuery()
{
    return !bIsStateActive && bCanGrapple;
}

void USR_GrapplingHookComponent::UpdateState()
{
}

FName USR_GrapplingHookComponent::GetStateName() const
{
    return FName("GrapplingHook");
}

int32 USR_GrapplingHookComponent::GetStatePriority() const
{
    return 1;
}

bool USR_GrapplingHookComponent::IsStateActive() const
{
    return bIsStateActive;
}

float USR_GrapplingHookComponent::GetDistanceToGrapplePoint() const
{
    return FVector::Distance(OwnerCharacter->GetActorLocation(), GrapplePoints);
}

void USR_GrapplingHookComponent::SetupCableVisuals()
{
    if (bShowDebugCable)
    {
        DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), GrapplePoints, 
                     FColor::White, false, 0.01f, 0, 2.0f);
    }
}

void USR_GrapplingHookComponent::UpdateCableVisuals()
{
    if (bShowDebugCable && bIsStateActive)
    {
        DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), GrapplePoints, 
                     FColor::White, false, 0.01f, 0, 2.0f);
        
        if (ActiveGrapplePoint)
        {
            DrawDebugSphere(GetWorld(), ActiveGrapplePoint->GetActorLocation(), 30.0f, 
                          12, FColor::Yellow, false, 0.01f);
        }
    }
}

void USR_GrapplingHookComponent::HideCableVisuals()
{
    // Les lignes debug avec durée courte disparaîtront automatiquement
}