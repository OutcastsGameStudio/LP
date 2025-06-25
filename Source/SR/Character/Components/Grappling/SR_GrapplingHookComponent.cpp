#include "SR_GrapplingHookComponent.h"
#include "SR/Character/SR_Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "SR/GameplayObjects/SR_GrapplePoint.h"

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
    
    CharacterMovement = OwnerCharacter->GetCharacterMovement();
    ContextStateComponent = OwnerCharacter->FindComponentByClass<USR_ContextStateComponent>();

    if (!CharacterMovement || !ContextStateComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load components in USR_GrapplingHookComponent::BeginPlay()"));
        return;
    }
    
    OwnerCharacter->OnGrappleInputPressed.AddDynamic(this, &USR_GrapplingHookComponent::OnGrappleInputPressed);
    OwnerCharacter->OnGrappleInputReleased.AddDynamic(this, &USR_GrapplingHookComponent::OnGrappleInputReleased);
}

void USR_GrapplingHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
                                              FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Gestion du cooldown
    if (!bCanGrapple)
    {
        CurrentCooldownTime += DeltaTime;
        if (CurrentCooldownTime >= GrappleCooldown)
        {
            bCanGrapple = true;
            CurrentCooldownTime = 0.0f;
        }
    }
    
    // Détection des points de grappin
    if (!bIsStateActive)
    {
        UpdateGrapplePointDetection();
    }
    
    // Mise à jour selon la phase
    if (bIsStateActive)
    {
        if (bIsPulling)
        {
            UpdatePullPhase(DeltaTime);
        }
        else if (bIsSwinging)
        {
            UpdateSwingPhase(DeltaTime);
        }
        
        if (bShowDebugInfo)
        {
            DrawDebugInfo();
        }
        
        if (ShouldReleaseGrapple())
        {
            ReleaseGrapple();
        }
    }
}

void USR_GrapplingHookComponent::LaunchGrapple()
{
    if (!bCanGrapple || bIsStateActive || !CurrentTargetedPoint)
    {
        return;
    }
    
    ActiveGrapplePoint = CurrentTargetedPoint;
    GrapplePoint = ActiveGrapplePoint->GetGrappleLocation();
    
    FGrappleData GrappleData;
    GrappleData.GrapplePoint = GrapplePoint;
    GrappleData.CableLength = FVector::Distance(OwnerCharacter->GetActorLocation(), GrapplePoint);
    GrappleData.TargetGrapplePoint = ActiveGrapplePoint;
    
    ContextStateComponent->TransitionState(MotionState::GRAPPLING_HOOK, &GrappleData);
}

void USR_GrapplingHookComponent::ReleaseGrapple()
{
    if (bIsStateActive)
    {
        LeaveState(0, true);
    }
}

void USR_GrapplingHookComponent::LaunchFromGrapple()
{
    if (!bIsStateActive || !bIsSwinging)
    {
        return;
    }
    
    // Calculer la vélocité de lancement
    FVector LaunchVelocity = PendulumState.Velocity;
    
    // Appliquer le multiplicateur de boost
    LaunchVelocity *= LaunchBoostMultiplier;
    
    // Ajouter un boost vertical si on se balance
    if (LaunchVelocity.Size() > 100.0f)
    {
        LaunchVelocity.Z += LaunchUpwardBoost;
    }
    
    // Sauvegarder la vélocité de lancement
    CharacterMovement->Velocity = LaunchVelocity;
    
    // Broadcast l'event
    OnGrapplingLaunched.Broadcast();
    
    // Relâcher le grappin
    LeaveState(0, true);
}

void USR_GrapplingHookComponent::EnterState(void* Data)
{
    if (bIsStateActive)
    {
        return;
    }
    
    FGrappleData* GrappleData = static_cast<FGrappleData*>(Data);
    if (GrappleData)
    {
        GrapplePoint = GrappleData->GrapplePoint;
        CableLength = GrappleData->CableLength;
        ActiveGrapplePoint = GrappleData->TargetGrapplePoint;
    }
    
    bIsStateActive = true;
    bCanGrapple = false;
    CurrentCooldownTime = 0.0f;
    
    // Sauvegarder la vélocité actuelle
    PreGrappleVelocity = CharacterMovement->Velocity;
    
    // Désactiver le mouvement normal du personnage
    CharacterMovement->SetMovementMode(MOVE_Flying);
    CharacterMovement->StopMovementImmediately();
    
    if (ActiveGrapplePoint)
    {
        ActiveGrapplePoint->OnGrappleAttached(OwnerCharacter);
    }
    
    StartGrapplePull();
    
    OnGrapplingStarted.Broadcast();
}

void USR_GrapplingHookComponent::StartGrapplePull()
{
    bIsPulling = true;
    bIsSwinging = false;
    PullProgress = 0.0f;
}

void USR_GrapplingHookComponent::UpdatePullPhase(float DeltaTime)
{
    // Calculer la progression de la traction (0 à 1)
    float PullDuration = 0.3f; // 300ms pour la traction
    PullProgress += DeltaTime / PullDuration;
    
    if (PullProgress >= 1.0f)
    {
        PullProgress = 1.0f;
        StartSwingPhase();
        return;
    }
    
    // Interpolation cubique pour un mouvement smooth
    float Alpha = PullProgress * PullProgress * (3.0f - 2.0f * PullProgress);
    
    // Position de départ et d'arrivée
    FVector StartPos = OwnerCharacter->GetActorLocation();
    FVector Direction = (GrapplePoint - StartPos).GetSafeNormal();
    FVector TargetPos = GrapplePoint - Direction * CableLength;
    
    // Calculer la nouvelle position
    FVector NewPosition = FMath::Lerp(StartPos, TargetPos, Alpha);
    
    // Calculer la vélocité pour la transition
    FVector DeltaPos = NewPosition - OwnerCharacter->GetActorLocation();
    PendulumState.Velocity = DeltaPos / DeltaTime;
    
    // Appliquer la position
    SetCharacterPosition(NewPosition);
}

void USR_GrapplingHookComponent::StartSwingPhase()
{
    bIsPulling = false;
    bIsSwinging = true;
    
    InitializePendulumState();
}

void USR_GrapplingHookComponent::InitializePendulumState()
{
    // Position relative au point d'ancrage
    FVector RelativePos = OwnerCharacter->GetActorLocation() - GrapplePoint;
    PendulumState.RelativePosition = RelativePos;
    
    // Convertir en coordonnées sphériques
    float Theta, Phi;
    CartesianToSpherical(RelativePos, Theta, Phi);
    PendulumState.Theta = Theta;
    PendulumState.Phi = Phi;
    
    // Calculer les vitesses angulaires initiales depuis la vélocité cartésienne
    FVector RadialDir = RelativePos.GetSafeNormal();
    FVector TangentVelocity = PendulumState.Velocity - 
        FVector::DotProduct(PendulumState.Velocity, RadialDir) * RadialDir;
    
    // Décomposer la vitesse tangentielle en composantes θ et φ
    FVector ThetaDir = FVector::CrossProduct(FVector::CrossProduct(RadialDir, FVector::UpVector), RadialDir).GetSafeNormal();
    FVector PhiDir = FVector::CrossProduct(FVector::UpVector, RadialDir).GetSafeNormal();
    
    float VTheta = FVector::DotProduct(TangentVelocity, ThetaDir);
    float VPhi = FVector::DotProduct(TangentVelocity, PhiDir);
    
    PendulumState.ThetaDot = VTheta / CableLength;
    PendulumState.PhiDot = VPhi / (CableLength * FMath::Sin(Theta));
    
    // Calculer l'énergie totale du système
    float KineticEnergy = 0.5f * CableLength * CableLength * 
        (PendulumState.ThetaDot * PendulumState.ThetaDot + 
         PendulumState.PhiDot * PendulumState.PhiDot * FMath::Sin(Theta) * FMath::Sin(Theta));
    float PotentialEnergy = Gravity * CableLength * (1.0f - FMath::Cos(PendulumState.Theta));
    PendulumState.TotalEnergy = KineticEnergy + PotentialEnergy;
}

void USR_GrapplingHookComponent::UpdateSwingPhase(float DeltaTime)
{
    // Accumuler le temps pour des pas fixes
    static float AccumulatedTime = 0.0f;
    AccumulatedTime += DeltaTime;
    
    // Intégration avec pas de temps fixe pour la stabilité
    while (AccumulatedTime >= PhysicsTimestep)
    {
        UpdatePendulumPhysics(PhysicsTimestep);
        AccumulatedTime -= PhysicsTimestep;
    }
    
    // Dessiner la trajectoire prédite si activé
    if (bShowTrajectory)
    {
        DrawTrajectory();
    }
}

void USR_GrapplingHookComponent::UpdatePendulumPhysics(float DeltaTime)
{
    // Équations du pendule sphérique:
    // θ'' = sin(θ)cos(θ)φ'² - (g/L)sin(θ)
    // φ'' = -2(θ'/tan(θ))φ'
    
    float SinTheta = FMath::Sin(PendulumState.Theta);
    float CosTheta = FMath::Cos(PendulumState.Theta);
    
    // Accélération θ (mouvement vertical)
    float ThetaAccel = SinTheta * CosTheta * PendulumState.PhiDot * PendulumState.PhiDot 
                      - (Gravity / CableLength) * SinTheta;
    
    // Accélération φ (mouvement horizontal)
    float PhiAccel = 0.0f;
    if (FMath::Abs(SinTheta) > 0.01f) // Éviter division par zéro
    {
        PhiAccel = -2.0f * (PendulumState.ThetaDot * CosTheta / SinTheta) * PendulumState.PhiDot;
    }
    
    // Ajouter l'amortissement
    ThetaAccel -= AirDamping * PendulumState.ThetaDot;
    PhiAccel -= AirDamping * PendulumState.PhiDot;
    
    // Appliquer l'input du joueur
    if (bIsButtonHeld)
    {
        ApplyPlayerInput(DeltaTime);
    }
    
    // Intégration de Verlet pour θ
    float OldTheta = PendulumState.Theta;
    PendulumState.Theta += PendulumState.ThetaDot * DeltaTime + 0.5f * ThetaAccel * DeltaTime * DeltaTime;
    
    // Limiter l'angle maximum
    float MaxAngleRad = FMath::DegreesToRadians(MaxSwingAngle);
    PendulumState.Theta = FMath::Clamp(PendulumState.Theta, 0.01f, MaxAngleRad); // Min 0.01 pour éviter singularité
    
    // Mettre à jour la vitesse angulaire θ
    float NewThetaDot = (PendulumState.Theta - OldTheta) / DeltaTime;
    PendulumState.ThetaDot = NewThetaDot;
    
    // Intégration pour φ
    PendulumState.Phi += PendulumState.PhiDot * DeltaTime + 0.5f * PhiAccel * DeltaTime * DeltaTime;
    PendulumState.PhiDot += PhiAccel * DeltaTime;
    
    // Appliquer la perte d'énergie
    PendulumState.ThetaDot *= (1.0f - EnergyLoss * DeltaTime);
    PendulumState.PhiDot *= (1.0f - EnergyLoss * DeltaTime);
    
    // Convertir en position cartésienne
    IntegrateMotion(DeltaTime);
}

void USR_GrapplingHookComponent::ApplyPlayerInput(float DeltaTime)
{
    FVector InputVector = CharacterMovement->GetLastInputVector();
    if (InputVector.IsNearlyZero())
    {
        return;
    }
    
    // Obtenir la caméra pour la direction de l'input
    UCameraComponent* Camera = OwnerCharacter->GetFollowCamera();
    if (!Camera)
        return;
    
    // Convertir l'input en espace monde basé sur la caméra
    FVector CameraForward = Camera->GetForwardVector();
    FVector CameraRight = Camera->GetRightVector();
    CameraForward.Z = 0;
    CameraRight.Z = 0;
    CameraForward.Normalize();
    CameraRight.Normalize();
    
    FVector WorldInput = CameraForward * InputVector.Y + CameraRight * InputVector.X;
    WorldInput.Normalize();
    
    // Position actuelle relative au point d'ancrage
    FVector CurrentPos = OwnerCharacter->GetActorLocation();
    FVector RadialDir = (CurrentPos - GrapplePoint).GetSafeNormal();
    
    // Calculer les directions tangentielles pour θ et φ
    FVector ThetaDir = FVector::CrossProduct(FVector::CrossProduct(RadialDir, FVector::UpVector), RadialDir).GetSafeNormal();
    FVector PhiDir = FVector::CrossProduct(FVector::UpVector, RadialDir).GetSafeNormal();
    
    // Projeter l'input sur les directions tangentielles
    float InputTheta = FVector::DotProduct(WorldInput, ThetaDir);
    float InputPhi = FVector::DotProduct(WorldInput, PhiDir);
    
    // Appliquer les forces d'input
    // Pour θ (avant/arrière) - plus fort pour permettre de gagner de la hauteur
    float ThetaInputAccel = (InputTheta * PlayerInputStrength) / CableLength;
    PendulumState.ThetaDot += ThetaInputAccel * DeltaTime;
    
    // Pour φ (gauche/droite) - contrôle directionnel
    if (FMath::Abs(FMath::Sin(PendulumState.Theta)) > 0.01f)
    {
        float PhiInputAccel = (InputPhi * DirectionalControlStrength) / (CableLength * FMath::Sin(PendulumState.Theta));
        PendulumState.PhiDot += PhiInputAccel * DeltaTime;
    }
    
    // Limiter les vitesses maximales pour éviter l'instabilité
    float MaxThetaDot = 5.0f; // rad/s
    float MaxPhiDot = 3.0f;   // rad/s
    PendulumState.ThetaDot = FMath::Clamp(PendulumState.ThetaDot, -MaxThetaDot, MaxThetaDot);
    PendulumState.PhiDot = FMath::Clamp(PendulumState.PhiDot, -MaxPhiDot, MaxPhiDot);
}

void USR_GrapplingHookComponent::IntegrateMotion(float DeltaTime)
{
    // Calculer la nouvelle position basée sur les angles θ et φ
    FVector NewRelativePos = CalculateNewPosition(PendulumState.Theta, PendulumState.Phi);
    FVector NewWorldPos = GrapplePoint + NewRelativePos;
    
    // Calculer la vélocité
    FVector DeltaPos = NewWorldPos - OwnerCharacter->GetActorLocation();
    PendulumState.Velocity = DeltaPos / DeltaTime;
    
    // Appliquer la contrainte de longueur
    EnforceCableConstraint();
    
    // Mettre à jour la position du personnage
    SetCharacterPosition(NewWorldPos);
    SetCharacterVelocity(PendulumState.Velocity);
}

FVector USR_GrapplingHookComponent::CalculateNewPosition(float Theta, float Phi)
{
    // Pendule sphérique 3D
    float X = CableLength * FMath::Sin(Theta) * FMath::Cos(Phi);
    float Y = CableLength * FMath::Sin(Theta) * FMath::Sin(Phi);
    float Z = -CableLength * FMath::Cos(Theta);
    
    return FVector(X, Y, Z);
}

void USR_GrapplingHookComponent::CartesianToSpherical(const FVector& CartesianPos, float& OutTheta, float& OutPhi)
{
    float R = CartesianPos.Size();
    if (R == 0)
    {
        OutTheta = 0;
        OutPhi = 0;
        return;
    }
    
    // Theta : angle avec l'axe Z (vertical)
    OutTheta = FMath::Acos(-CartesianPos.Z / R);
    
    // Phi : angle dans le plan XY
    OutPhi = FMath::Atan2(CartesianPos.Y, CartesianPos.X);
}

FVector USR_GrapplingHookComponent::SphericalToCartesian(float Theta, float Phi)
{
    float X = CableLength * FMath::Sin(Theta) * FMath::Cos(Phi);
    float Y = CableLength * FMath::Sin(Theta) * FMath::Sin(Phi);
    float Z = -CableLength * FMath::Cos(Theta);
    
    return FVector(X, Y, Z);
}

void USR_GrapplingHookComponent::EnforceCableConstraint()
{
    FVector CurrentPos = OwnerCharacter->GetActorLocation();
    FVector ToAnchor = CurrentPos - GrapplePoint;
    float CurrentDistance = ToAnchor.Size();
    
    if (CurrentDistance > CableLength)
    {
        // Ramener à la bonne distance
        FVector ConstrainedPos = GrapplePoint + ToAnchor.GetSafeNormal() * CableLength;
        SetCharacterPosition(ConstrainedPos);
        
        // Ajuster la vélocité pour qu'elle soit tangentielle
        FVector RadialDir = ToAnchor.GetSafeNormal();
        PendulumState.Velocity -= FVector::DotProduct(PendulumState.Velocity, RadialDir) * RadialDir;
    }
}

void USR_GrapplingHookComponent::SetCharacterPosition(const FVector& NewPosition)
{
    OwnerCharacter->SetActorLocation(NewPosition, true);
}

void USR_GrapplingHookComponent::SetCharacterVelocity(const FVector& NewVelocity)
{
    CharacterMovement->Velocity = NewVelocity;
}

bool USR_GrapplingHookComponent::ShouldReleaseGrapple()
{
    if (bContinuousSwingMode && bIsButtonHeld)
    {
        return false;
    }
    
    if (CharacterMovement->IsMovingOnGround())
    {
        return true;
    }
    
    if (FVector::Distance(OwnerCharacter->GetActorLocation(), GrapplePoint) < 100.0f)
    {
        return true;
    }
    
    return false;
}

void USR_GrapplingHookComponent::LeaveState(int32 RootMotionId, bool bForced)
{
    bIsStateActive = false;
    bIsSwinging = false;
    bIsPulling = false;
    bIsButtonHeld = false;
    
    // Restaurer le mode de mouvement normal
    if (CharacterMovement->IsMovingOnGround())
    {
        CharacterMovement->SetMovementMode(MOVE_Walking);
    }
    else
    {
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }
    
    // Conserver la vélocité du pendule
    CharacterMovement->Velocity = PendulumState.Velocity;
    
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

void USR_GrapplingHookComponent::DrawDebugInfo()
{
    if (!bShowDebugInfo)
        return;
    
    FVector CharPos = OwnerCharacter->GetActorLocation();
    
    // Ligne du câble
    DrawDebugLine(GetWorld(), CharPos, GrapplePoint, FColor::White, false, 0.01f, 0, 2.0f);
    
    // Point d'ancrage
    DrawDebugSphere(GetWorld(), GrapplePoint, 20.0f, 12, FColor::Yellow, false, 0.01f);
    
    // Informations textuelles
    if (bIsSwinging)
    {
        FString InfoText = FString::Printf(
            TEXT("θ: %.1f° | φ: %.1f°\nVitesse: %.1f cm/s\nÉnergie: %.1f J"),
            FMath::RadiansToDegrees(PendulumState.Theta),
            FMath::RadiansToDegrees(PendulumState.Phi),
            PendulumState.Velocity.Size(),
            PendulumState.TotalEnergy
        );
        
        DrawDebugString(GetWorld(), CharPos + FVector(0, 0, 100), InfoText, 
                       nullptr, FColor::Green, 0.01f, true, 1.0f);
        
        // Afficher la direction de la vélocité
        DrawDebugLine(GetWorld(), CharPos, CharPos + PendulumState.Velocity * 0.2f,
                     FColor::Blue, false, 0.01f, 0, 3.0f);
    }
}

void USR_GrapplingHookComponent::DrawTrajectory()
{
    if (!bShowTrajectory || !bIsSwinging)
        return;
    
    // Simuler la trajectoire future
    float SimTheta = PendulumState.Theta;
    float SimPhi = PendulumState.Phi;
    float SimThetaDot = PendulumState.ThetaDot;
    float SimPhiDot = PendulumState.PhiDot;
    FVector LastPos = OwnerCharacter->GetActorLocation();
    
    for (int i = 0; i < 50; i++)
    {
        // Simulation simplifiée du pendule sphérique
        float SinTheta = FMath::Sin(SimTheta);
        float CosTheta = FMath::Cos(SimTheta);
        
        float SimThetaAccel = SinTheta * CosTheta * SimPhiDot * SimPhiDot 
                            - (Gravity / CableLength) * SinTheta;
        float SimPhiAccel = 0.0f;
        if (FMath::Abs(SinTheta) > 0.01f)
        {
            SimPhiAccel = -2.0f * (SimThetaDot * CosTheta / SinTheta) * SimPhiDot;
        }
        
        SimThetaDot += SimThetaAccel * 0.1f;
        SimPhiDot += SimPhiAccel * 0.1f;
        SimTheta += SimThetaDot * 0.1f;
        SimPhi += SimPhiDot * 0.1f;
        
        FVector SimPos = GrapplePoint + CalculateNewPosition(SimTheta, SimPhi);
        
        DrawDebugLine(GetWorld(), LastPos, SimPos, FColor::Red, false, 0.01f, 0, 1.0f);
        LastPos = SimPos;
    }
}

// Méthodes de détection (restent identiques)
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
        return nullptr;
    
    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector CameraForward = FollowCamera->GetForwardVector();
    
    for (AActor* Actor : FoundGrapplePoints)
    {
        ASR_GrapplePoint* GrapplePoints = Cast<ASR_GrapplePoint>(Actor);
        if (!GrapplePoints || !GrapplePoints->IsActive())
            continue;
        
        FVector ToGrapplePoint = GrapplePoints->GetActorLocation() - CameraLocation;
        float Distance = ToGrapplePoint.Size();
        
        if (Distance > MaxGrappleDistance)
            continue;
        
        ToGrapplePoint.Normalize();
        float DotProduct = FVector::DotProduct(CameraForward, ToGrapplePoint);
        float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
        
        if (Angle > GrappleDetectionAngle)
            continue;
        
        if (!GrapplePoints->CanBeGrappled(OwnerCharacter->GetActorLocation()))
            continue;
        
        if (!IsGrapplePointInView(GrapplePoints))
            continue;
        
        float Score = DotProduct * (1.0f - (Distance / MaxGrappleDistance));
        
        if (Score > BestScore)
        {
            BestScore = Score;
            BestGrapplePoint = GrapplePoints;
        }
    }
    
    return BestGrapplePoint;
}

bool USR_GrapplingHookComponent::IsGrapplePointInView(ASR_GrapplePoint* GrapplePointActor) const
{
    if (!GrapplePointActor || !OwnerCharacter)
        return false;
    
    UCameraComponent* FollowCamera = OwnerCharacter->GetFollowCamera();
    if (!FollowCamera)
        return false;
    
    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector TargetLocation = GrapplePointActor->GetGrappleLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);
    QueryParams.AddIgnoredActor(GrapplePointActor);
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, CameraLocation, TargetLocation,
        ECC_Visibility, QueryParams
    );
    
    return !bHit;
}

void USR_GrapplingHookComponent::OnGrappleInputPressed()
{
    bIsButtonHeld = true;
    
    if (!bIsStateActive)
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

// Méthodes de l'interface ISR_State
void USR_GrapplingHookComponent::UpdateState() {}
bool USR_GrapplingHookComponent::LookAheadQuery() { return !bIsStateActive && bCanGrapple; }
FName USR_GrapplingHookComponent::GetStateName() const { return FName("GrapplingHook"); }
int32 USR_GrapplingHookComponent::GetStatePriority() const { return 1; }
bool USR_GrapplingHookComponent::IsStateActive() const { return bIsStateActive; }