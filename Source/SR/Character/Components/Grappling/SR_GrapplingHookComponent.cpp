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
    
    // Connexion aux événements d'input
    OwnerCharacter->OnGrappleInputPressed.AddDynamic(this, &USR_GrapplingHookComponent::OnGrappleInputPressed);
    OwnerCharacter->OnGrappleInputReleased.AddDynamic(this, &USR_GrapplingHookComponent::OnGrappleInputReleased);
    
    // Nouveaux bindings pour les inputs avant/arrière
    OwnerCharacter->OnMoveForwardInputPressed.AddDynamic(this, &USR_GrapplingHookComponent::OnMoveForwardInputPressed);
    OwnerCharacter->OnMoveForwardInputReleased.AddDynamic(this, &USR_GrapplingHookComponent::OnMoveForwardInputReleased);
    // Note: Vous devrez ajouter des événements pour reculer dans SR_Character si nécessaire
}

void USR_GrapplingHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
                                              FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Gestion du cooldown du grappin
    if (!bCanGrapple)
    {
        CurrentCooldownTime += DeltaTime;
        if (CurrentCooldownTime >= GrappleCooldown)
        {
            bCanGrapple = true;
            CurrentCooldownTime = 0.0f;
        }
    }
    
    // Gestion du cooldown d'impulsion
    if (!bCanApplyImpulse)
    {
        float TimeSinceLastImpulse = GetWorld()->GetTimeSeconds() - LastImpulseTime;
        if (TimeSinceLastImpulse >= ImpulseCooldown)
        {
            bCanApplyImpulse = true;
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
    
    // Récupérer la vélocité actuelle du pendule
    FVector CurrentVelocity = PendulumState.Velocity;
    
    // S'assurer qu'on a une vélocité significative
    if (CurrentVelocity.Size() < 100.0f)
    {
        // Si la vélocité est trop faible, utiliser la direction du personnage
        CurrentVelocity = OwnerCharacter->GetActorForwardVector() * 500.0f;
    }
    
    // Normaliser la direction et appliquer le boost
    FVector LaunchDirection = CurrentVelocity.GetSafeNormal();
    float CurrentSpeed = CurrentVelocity.Size();
    
    // Calculer la vélocité de lancement
    // Utiliser la vitesse actuelle multipliée par le boost
    FVector LaunchVelocity = LaunchDirection * (CurrentSpeed * LaunchBoostMultiplier);
    
    // Ajouter un boost vertical si on se balance vers le haut
    if (LaunchDirection.Z > 0.1f)
    {
        // Si on va déjà vers le haut, ajouter plus de boost vertical
        LaunchVelocity.Z += LaunchUpwardBoost * 1.5f;
    }
    else if (LaunchDirection.Z > -0.5f)
    {
        // Si on va horizontalement ou légèrement vers le bas, ajouter un boost vertical normal
        LaunchVelocity.Z += LaunchUpwardBoost;
    }
    else
    {
        // Si on va fortement vers le bas, ajouter moins de boost vertical
        LaunchVelocity.Z += LaunchUpwardBoost * 0.5f;
    }
    
    // Option : Ajouter un boost supplémentaire basé sur l'amplitude du balancement
    if (bUseAmplitudeBoost)
    {
        float AmplitudeBonus = FMath::Clamp(CurrentSwingAmplitude / MaxSwingAmplitude, 0.0f, 1.0f);
        LaunchVelocity *= (1.0f + AmplitudeBonus * MaxAmplitudeBoostPercent);
    }
    
    // Appliquer la vélocité de lancement au mouvement du personnage
    CharacterMovement->Velocity = LaunchVelocity;
    
    // S'assurer que le personnage est orienté dans la bonne direction
    if (!LaunchDirection.IsNearlyZero())
    {
        FRotator NewRotation = LaunchDirection.Rotation();
        NewRotation.Pitch = 0; // Garder le personnage droit
        OwnerCharacter->SetActorRotation(NewRotation);
    }
    
    // Broadcast l'event
    OnGrapplingLaunched.Broadcast();
    
    // Debug : Afficher la direction et la force du lancement
    if (bShowDebugInfo)
    {
        FVector CharPos = OwnerCharacter->GetActorLocation();
        DrawDebugLine(GetWorld(), CharPos, CharPos + LaunchDirection * 500.0f, 
                     FColor::Yellow, false, 2.0f, 0, 5.0f);
        
        FString LaunchInfo = FString::Printf(
            TEXT("Launch Speed: %.1f cm/s\nLaunch Angle: %.1f°"),
            LaunchVelocity.Size(),
            FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(LaunchDirection.Z, -1.0f, 1.0f)))
        );
        
        DrawDebugString(GetWorld(), CharPos + FVector(0, 0, 200), LaunchInfo, 
                       nullptr, FColor::Yellow, 2.0f, true, 2.0f);
    }
    
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
    
    // Calculer la longueur de balancement
    SwingCableLength = CableLength * SwingDistanceRatio;
    
    bIsStateActive = true;
    bCanGrapple = false;
    CurrentCooldownTime = 0.0f;
    bCanApplyImpulse = true;
    CurrentSwingAmplitude = 0.0f;
    
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
    float PullDuration = 0.5f; // 500ms pour la traction
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
    
    // S'arrêter à la distance de balancement au lieu d'aller jusqu'au point
    FVector TargetPos = GrapplePoint - Direction * SwingCableLength;
    
    // Déterminer le plan de balancement dès le début du pull
    if (PullProgress < 0.1f) // Premier frame du pull
    {
        FVector CharacterForward = OwnerCharacter->GetActorForwardVector();
        FVector SwingDirection = CharacterForward;
        SwingDirection.Z = 0;
        SwingDirection.Normalize();
        
        if (SwingDirection.IsNearlyZero())
        {
            SwingDirection = FVector(Direction.X, Direction.Y, 0).GetSafeNormal();
            if (SwingDirection.IsNearlyZero())
            {
                SwingDirection = FVector::ForwardVector;
            }
        }
        
        PendulumState.SwingForward = SwingDirection;
        PendulumState.SwingPlaneNormal = FVector::CrossProduct(SwingDirection, FVector::UpVector).GetSafeNormal();
    }
    
    // Projeter la position cible sur le plan de balancement
    FVector ToTarget = TargetPos - GrapplePoint;
    float DistanceFromPlane = FVector::DotProduct(ToTarget, PendulumState.SwingPlaneNormal);
    TargetPos -= DistanceFromPlane * PendulumState.SwingPlaneNormal;
    
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
    
    // Conserver la vélocité du pull avant d'initialiser le pendule
    FVector PullVelocity = PendulumState.Velocity;
    
    InitializePendulumState();
    
    // Si la vélocité du pull était significative, l'utiliser pour déterminer la vitesse angulaire initiale
    if (PullVelocity.Size() > 100.0f)
    {
        // Projeter la vélocité du pull dans le plan de balancement
        FVector ProjectedVelocity = PullVelocity - FVector::DotProduct(PullVelocity, PendulumState.SwingPlaneNormal) * PendulumState.SwingPlaneNormal;
        
        // Déterminer la composante tangentielle dans la direction du swing
        FVector RelativePos = OwnerCharacter->GetActorLocation() - GrapplePoint;
        FVector RadialDir = RelativePos.GetSafeNormal();
        FVector TangentDir = FVector::CrossProduct(PendulumState.SwingPlaneNormal, RadialDir);
        
        float TangentSpeed = FVector::DotProduct(ProjectedVelocity, TangentDir);
        PendulumState.AngularVelocity = TangentSpeed / SwingCableLength;
        
        // S'assurer que la vitesse angulaire est dans la bonne direction
        if (FMath::Abs(PendulumState.AngularVelocity) < 0.05f)
        {
            PendulumState.AngularVelocity = FMath::Sign(PendulumState.Angle) * 0.2f;
        }
    }
}

void USR_GrapplingHookComponent::InitializePendulumState()
{
    // Position relative au point d'ancrage
    FVector RelativePos = OwnerCharacter->GetActorLocation() - GrapplePoint;
    PendulumState.RelativePosition = RelativePos;
    
    // Déterminer le plan de balancement basé sur la direction initiale
    FVector CharacterForward = OwnerCharacter->GetActorForwardVector();
    FVector ToGrapplePoint = (GrapplePoint - OwnerCharacter->GetActorLocation()).GetSafeNormal();
    
    // Le plan de balancement est défini par la direction du personnage
    // On projette cette direction sur le plan horizontal
    FVector SwingDirection = CharacterForward;
    SwingDirection.Z = 0;
    SwingDirection.Normalize();
    
    // Si on n'a pas de direction claire, utiliser la direction vers le point de grappin
    if (SwingDirection.IsNearlyZero())
    {
        SwingDirection = FVector(ToGrapplePoint.X, ToGrapplePoint.Y, 0).GetSafeNormal();
        if (SwingDirection.IsNearlyZero())
        {
            SwingDirection = FVector::ForwardVector;
        }
    }
    
    // Définir le plan de balancement
    PendulumState.SwingForward = SwingDirection;
    PendulumState.SwingPlaneNormal = FVector::CrossProduct(SwingDirection, FVector::UpVector).GetSafeNormal();
    
    // Calculer l'angle initial dans le plan de balancement
    FVector ProjPos = RelativePos - FVector::DotProduct(RelativePos, PendulumState.SwingPlaneNormal) * PendulumState.SwingPlaneNormal;
    float SignedDistance = FVector::DotProduct(ProjPos, PendulumState.SwingForward);
    float VerticalDistance = -RelativePos.Z;
    
    PendulumState.Angle = FMath::Atan2(SignedDistance, VerticalDistance);
    
    // S'assurer qu'on n'est pas exactement en bas (singularité)
    if (FMath::Abs(PendulumState.Angle) < 0.1f)
    {
        PendulumState.Angle = 0.1f * FMath::Sign(SignedDistance + 0.01f);
        
        // Ajouter une vitesse initiale si nécessaire
        if (PreGrappleVelocity.Size() < 100.0f)
        {
            PendulumState.AngularVelocity = FMath::RandRange(0.05f, 0.2f);
        }
    }
    
    // Calculer la vitesse angulaire initiale
    if (!PreGrappleVelocity.IsZero())
    {
        // Projeter la vitesse sur le plan de balancement
        FVector TangentDir = FVector::CrossProduct(PendulumState.SwingPlaneNormal, -RelativePos.GetSafeNormal());
        float TangentSpeed = FVector::DotProduct(PreGrappleVelocity, TangentDir);
        PendulumState.AngularVelocity = TangentSpeed / SwingCableLength;
    }
    else if (!PendulumState.Velocity.IsZero())
    {
        // Utiliser la vitesse de la phase de pull
        FVector TangentDir = FVector::CrossProduct(PendulumState.SwingPlaneNormal, -RelativePos.GetSafeNormal());
        float TangentSpeed = FVector::DotProduct(PendulumState.Velocity, TangentDir);
        PendulumState.AngularVelocity = TangentSpeed / SwingCableLength;
    }
    
    // S'assurer qu'il y a toujours un minimum de mouvement
    if (FMath::Abs(PendulumState.AngularVelocity) < 0.1f)
    {
        PendulumState.AngularVelocity = FMath::RandRange(0.05f, 0.2f) * FMath::Sign(PendulumState.Angle);
    }
    
    // Calculer l'énergie totale du système
    float KineticEnergy = 0.5f * SwingCableLength * SwingCableLength * PendulumState.AngularVelocity * PendulumState.AngularVelocity;
    float PotentialEnergy = Gravity * SwingCableLength * (1.0f - FMath::Cos(PendulumState.Angle));
    PendulumState.TotalEnergy = KineticEnergy + PotentialEnergy;
}

void USR_GrapplingHookComponent::UpdateSwingPhase(float DeltaTime)
{
    // Vérifier si le joueur est bloqué (vitesse trop faible)
    float CurrentSpeed = PendulumState.Velocity.Size();
    if (CurrentSpeed < MinimumSwingSpeed)
    {
        // Ajouter une impulsion dans la direction du mouvement ou dans la direction avant
        if (FMath::Abs(PendulumState.AngularVelocity) > 0.1f)
        {
            // Augmenter la vitesse angulaire dans la direction actuelle
            PendulumState.AngularVelocity *= 1.5f;
        }
        else
        {
            // Si vraiment bloqué, donner une impulsion
            PendulumState.AngularVelocity = FMath::Sign(PendulumState.Angle + 0.01f) * 0.2f;
        }
    }
    
    // Accumuler le temps pour des pas fixes
    static float AccumulatedTime = 0.0f;
    AccumulatedTime += DeltaTime;
    
    // Intégration avec pas de temps fixe pour la stabilité
    while (AccumulatedTime >= PhysicsTimestep)
    {
        UpdatePendulumPhysics(PhysicsTimestep);
        AccumulatedTime -= PhysicsTimestep;
    }
    
    // Appliquer l'impulsion si demandée
    if (bCanApplyImpulse)
    {
        if (bIsPressingForward)
        {
            ApplyImpulse(true);
        }
        else if (bIsPressingBackward)
        {
            ApplyImpulse(false);
        }
    }
    
    // Dessiner la trajectoire prédite si activé
    if (bShowTrajectory)
    {
        DrawTrajectory();
    }
}

void USR_GrapplingHookComponent::UpdatePendulumPhysics(float DeltaTime)
{
    // Équation du pendule simple: θ'' = -(g/L)sin(θ)
    
    float SinAngle = FMath::Sin(PendulumState.Angle);
    
    // Vérifier si on est proche du point bas (singularité)
    bool bNearBottom = FMath::Abs(PendulumState.Angle) < 0.1f; // ~5.7 degrés
    
    // Accélération angulaire de base
    float AngularAccel = -(Gravity / SwingCableLength) * SinAngle;
    
    // Ajouter l'amortissement (léger pour garder le mouvement)
    AngularAccel -= AirDamping * PendulumState.AngularVelocity;
    
    // Appliquer une force de rappel progressive si on approche de la limite
    float MaxAngleRad = FMath::DegreesToRadians(MaxSwingAngle);
    float AngleLimitBuffer = 0.1f; // 10% de marge avant la limite
    float SoftLimitStart = MaxAngleRad * (1.0f - AngleLimitBuffer);
    
    if (FMath::Abs(PendulumState.Angle) > SoftLimitStart)
    {
        // Calculer la force de rappel progressive
        float OverLimit = (FMath::Abs(PendulumState.Angle) - SoftLimitStart) / (MaxAngleRad * AngleLimitBuffer);
        OverLimit = FMath::Clamp(OverLimit, 0.0f, 1.0f);
        
        // Force de rappel qui augmente exponentiellement près de la limite
        float RepelForce = OverLimit * OverLimit * 50.0f; // Force maximale de 50 rad/s²
        
        // Appliquer la force dans la direction opposée à l'angle
        AngularAccel -= FMath::Sign(PendulumState.Angle) * RepelForce;
        
        // Ajouter aussi un amortissement supplémentaire près de la limite
        AngularAccel -= PendulumState.AngularVelocity * OverLimit * 5.0f;
    }
    
    // Si on est bloqué au point bas, ajouter une petite perturbation
    if (bNearBottom && FMath::Abs(PendulumState.AngularVelocity) < 0.1f)
    {
        // Ajouter une petite vitesse pour sortir de la singularité
        PendulumState.AngularVelocity += FMath::RandRange(-0.5f, 0.5f);
    }
    
    // Intégration de Verlet
    float OldAngle = PendulumState.Angle;
    PendulumState.Angle += PendulumState.AngularVelocity * DeltaTime + 0.5f * AngularAccel * DeltaTime * DeltaTime;
    
    // Limite absolue de sécurité (ne devrait jamais être atteinte avec la force de rappel)
    PendulumState.Angle = FMath::Clamp(PendulumState.Angle, -MaxAngleRad * 1.1f, MaxAngleRad * 1.1f);
    
    // Mettre à jour la vitesse angulaire
    PendulumState.AngularVelocity += AngularAccel * DeltaTime;
    
    // Si on est vraiment à la limite absolue, inverser la vitesse
    if (FMath::Abs(PendulumState.Angle) >= MaxAngleRad)
    {
        PendulumState.Angle = FMath::Sign(PendulumState.Angle) * MaxAngleRad;
        if (PendulumState.AngularVelocity * PendulumState.Angle > 0) // Vitesse dans la mauvaise direction
        {
            PendulumState.AngularVelocity *= -0.8f; // Rebond avec perte d'énergie
        }
    }
    
    // Appliquer le steering latéral si activé
    if (bAllowSwingSteer && CharacterMovement)
    {
        FVector InputVector = CharacterMovement->GetLastInputVector();
        if (!InputVector.IsNearlyZero())
        {
            // Obtenir la caméra pour la direction de l'input
            UCameraComponent* Camera = OwnerCharacter->GetFollowCamera();
            if (Camera)
            {
                FVector CameraRight = Camera->GetRightVector();
                CameraRight.Z = 0;
                CameraRight.Normalize();
                
                // Calculer l'input latéral
                float LateralInput = FVector::DotProduct(InputVector, CameraRight);
                
                if (FMath::Abs(LateralInput) > 0.1f)
                {
                    // Faire pivoter légèrement le plan de balancement
                    FRotator SteerRotation(0, LateralInput * SwingSteerStrength * DeltaTime, 0);
                    PendulumState.SwingForward = SteerRotation.RotateVector(PendulumState.SwingForward);
                    PendulumState.SwingPlaneNormal = FVector::CrossProduct(PendulumState.SwingForward, FVector::UpVector).GetSafeNormal();
                }
            }
        }
    }
    
    // Convertir en position cartésienne
    IntegrateMotion(DeltaTime);
}

void USR_GrapplingHookComponent::ApplyImpulse(bool bForward)
{
    if (!bCanApplyImpulse)
    {
        return;
    }
    
    // Vérifier si on n'a pas atteint l'amplitude maximale pour les impulsions
    float MaxAmplitudeRad = FMath::DegreesToRadians(MaxSwingAmplitude);
    
    // Permettre les impulsions même près de la limite, mais avec efficacité réduite
    float AmplitudeRatio = FMath::Abs(PendulumState.Angle) / MaxAmplitudeRad;
    float EfficiencyFromAmplitude = 1.0f - (AmplitudeRatio * AmplitudeRatio); // Réduction quadratique
    
    if (EfficiencyFromAmplitude <= 0.1f)
    {
        // Trop proche de la limite, impulsion très faible
        return;
    }
    
    // Direction de l'impulsion basée sur le sens du mouvement actuel
    float ImpulseDirection = bForward ? 1.0f : -1.0f;
    
    // Si on pousse dans le sens du mouvement actuel, amplifier
    // Si on pousse contre le mouvement, réduire l'efficacité
    float EfficiencyFactor = 1.0f;
    if ((PendulumState.AngularVelocity > 0 && !bForward) || 
        (PendulumState.AngularVelocity < 0 && bForward))
    {
        EfficiencyFactor = 0.5f; // Moins efficace quand on va contre le mouvement
    }
    
    // Calculer l'impulsion angulaire avec toutes les efficacités
    float TotalEfficiency = EfficiencyFactor * EfficiencyFromAmplitude;
    float AngularImpulse = (ImpulseStrength / SwingCableLength) * ImpulseDirection * TotalEfficiency * 0.01f;
    
    // Appliquer l'impulsion
    PendulumState.AngularVelocity += AngularImpulse;
    
    // Limiter la vitesse angulaire maximale
    float MaxAngularVelocity = 5.0f; // rad/s
    PendulumState.AngularVelocity = FMath::Clamp(PendulumState.AngularVelocity, -MaxAngularVelocity, MaxAngularVelocity);
    
    // Désactiver l'impulsion et démarrer le cooldown
    bCanApplyImpulse = false;
    LastImpulseTime = GetWorld()->GetTimeSeconds();
    
    // Mettre à jour l'amplitude actuelle
    CurrentSwingAmplitude = FMath::RadiansToDegrees(FMath::Abs(PendulumState.Angle));
}

void USR_GrapplingHookComponent::IntegrateMotion(float DeltaTime)
{
    // Calculer la nouvelle position basée sur l'angle
    FVector NewRelativePos = CalculateNewPosition(PendulumState.Angle, 0);
    FVector NewWorldPos = GrapplePoint + NewRelativePos;
    
    // Calculer la vélocité
    FVector DeltaPos = NewWorldPos - OwnerCharacter->GetActorLocation();
    PendulumState.Velocity = DeltaPos / DeltaTime;
    
    // Appliquer la contrainte de longueur
    EnforceCableConstraint();
    
    // Conservation de l'énergie : s'assurer que le pendule ne perd pas d'énergie
    float CurrentKineticEnergy = 0.5f * SwingCableLength * SwingCableLength * 
        PendulumState.AngularVelocity * PendulumState.AngularVelocity;
    float CurrentPotentialEnergy = Gravity * SwingCableLength * (1.0f - FMath::Cos(PendulumState.Angle));
    float CurrentTotalEnergy = CurrentKineticEnergy + CurrentPotentialEnergy;
    
    // Si l'énergie a diminué de plus de 5%, la restaurer
    if (CurrentTotalEnergy < PendulumState.TotalEnergy * 0.95f && PendulumState.TotalEnergy > 0.0f)
    {
        float EnergyRatio = FMath::Sqrt(PendulumState.TotalEnergy / CurrentTotalEnergy);
        PendulumState.AngularVelocity *= EnergyRatio;
    }
    
    // Mettre à jour la position du personnage
    SetCharacterPosition(NewWorldPos);
    SetCharacterVelocity(PendulumState.Velocity);
    
    // Orienter le personnage dans la direction du mouvement si on a une vitesse significative
    if (PendulumState.Velocity.Size() > 100.0f)
    {
        FVector VelocityDir = PendulumState.Velocity.GetSafeNormal();
        FRotator NewRotation = VelocityDir.Rotation();
        NewRotation.Pitch = 0; // Garder le personnage droit
        OwnerCharacter->SetActorRotation(NewRotation);
    }
}

FVector USR_GrapplingHookComponent::CalculateNewPosition(float Angle, float Unused)
{
    // Pendule simple 2D dans le plan défini
    // Position dans le plan local du pendule
    float X = SwingCableLength * FMath::Sin(Angle);  // Distance horizontale dans la direction du swing
    float Z = -SwingCableLength * FMath::Cos(Angle); // Distance verticale (toujours négatif)
    
    // Convertir en coordonnées monde en utilisant le plan de balancement
    FVector LocalPos = PendulumState.SwingForward * X + FVector::UpVector * Z;
    
    return LocalPos;
}



void USR_GrapplingHookComponent::EnforceCableConstraint()
{
    FVector CurrentPos = OwnerCharacter->GetActorLocation();
    FVector ToAnchor = CurrentPos - GrapplePoint;
    float CurrentDistance = ToAnchor.Size();
    
    if (CurrentDistance > SwingCableLength)
    {
        // Ramener à la bonne distance
        FVector ConstrainedPos = GrapplePoint + ToAnchor.GetSafeNormal() * SwingCableLength;
        SetCharacterPosition(ConstrainedPos);
        
        // Ajuster la vélocité pour qu'elle soit tangentielle
        FVector RadialDir = ToAnchor.GetSafeNormal();
        PendulumState.Velocity -= FVector::DotProduct(PendulumState.Velocity, RadialDir) * RadialDir;
        
        // S'assurer que le personnage reste dans le plan de balancement
        FVector ToChar = ConstrainedPos - GrapplePoint;
        float DistanceFromPlane = FVector::DotProduct(ToChar, PendulumState.SwingPlaneNormal);
        if (FMath::Abs(DistanceFromPlane) > 10.0f) // Tolérance de 10cm
        {
            // Projeter la position sur le plan
            ConstrainedPos -= DistanceFromPlane * PendulumState.SwingPlaneNormal;
            SetCharacterPosition(ConstrainedPos);
            
            // Recalculer l'angle après la contrainte
            ToChar = ConstrainedPos - GrapplePoint;
            float SignedDistance = FVector::DotProduct(ToChar, PendulumState.SwingForward);
            float VerticalDistance = -ToChar.Z;
            PendulumState.Angle = FMath::Atan2(SignedDistance, VerticalDistance);
        }
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
    bIsPressingForward = false;
    bIsPressingBackward = false;
    
    // Conserver la vélocité du pendule et l'appliquer au personnage
    if (!CharacterMovement->Velocity.IsZero())
    {
        // La vélocité a déjà été définie (par LaunchFromGrapple)
    }
    else
    {
        // Sortie normale : conserver la direction et la vitesse actuelles
        CharacterMovement->Velocity = PendulumState.Velocity;
    }
    
    // Restaurer le mode de mouvement normal
    if (CharacterMovement->IsMovingOnGround())
    {
        CharacterMovement->SetMovementMode(MOVE_Walking);
    }
    else
    {
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }
    
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
    
    // Afficher le plan de balancement
    if (bIsSwinging)
    {
        // Dessiner le plan de balancement
        FVector PlanePoint1 = GrapplePoint + PendulumState.SwingForward * 100.0f;
        FVector PlanePoint2 = GrapplePoint - PendulumState.SwingForward * 100.0f;
        DrawDebugLine(GetWorld(), PlanePoint1, PlanePoint2, FColor::Cyan, false, 0.01f, 0, 2.0f);
        
        // Dessiner les limites d'angle
        float MaxAngleRad = FMath::DegreesToRadians(MaxSwingAngle);
        FVector LimitPos1 = GrapplePoint + CalculateNewPosition(MaxAngleRad, 0);
        FVector LimitPos2 = GrapplePoint + CalculateNewPosition(-MaxAngleRad, 0);
        DrawDebugLine(GetWorld(), GrapplePoint, LimitPos1, FColor::Red, false, 0.01f, 0, 1.0f);
        DrawDebugLine(GetWorld(), GrapplePoint, LimitPos2, FColor::Red, false, 0.01f, 0, 1.0f);
        
        // Zone de limite souple
        float SoftLimitAngle = MaxAngleRad * 0.9f;
        FVector SoftLimitPos1 = GrapplePoint + CalculateNewPosition(SoftLimitAngle, 0);
        FVector SoftLimitPos2 = GrapplePoint + CalculateNewPosition(-SoftLimitAngle, 0);
        DrawDebugLine(GetWorld(), GrapplePoint, SoftLimitPos1, FColor::Orange, false, 0.01f, 0, 1.0f);
        DrawDebugLine(GetWorld(), GrapplePoint, SoftLimitPos2, FColor::Orange, false, 0.01f, 0, 1.0f);
        
        // Informations textuelles
        FString InfoText = FString::Printf(
            TEXT("Angle: %.1f° (Max: %.1f°)\nVitesse: %.1f cm/s\nAmplitude: %.1f°\nImpulsion: %s\nÉnergie: %.1f J"),
            FMath::RadiansToDegrees(PendulumState.Angle),
            MaxSwingAngle,
            PendulumState.Velocity.Size(),
            CurrentSwingAmplitude,
            bCanApplyImpulse ? TEXT("Prête") : TEXT("Cooldown"),
            PendulumState.TotalEnergy
        );
        
        // Indicateur de proximité de la limite
        float AngleRatio = FMath::Abs(PendulumState.Angle) / MaxAngleRad;
        if (AngleRatio > 0.9f)
        {
            InfoText += FString::Printf(TEXT("\n⚠️ LIMITE: %.0f%%"), AngleRatio * 100.0f);
        }
        
        DrawDebugString(GetWorld(), CharPos + FVector(0, 0, 100), InfoText, 
                       nullptr, FColor::Green, 0.01f, true, 1.0f);
        
        // Afficher la direction de la vélocité
        DrawDebugLine(GetWorld(), CharPos, CharPos + PendulumState.Velocity * 0.2f,
                     FColor::Blue, false, 0.01f, 0, 3.0f);
        
        // Afficher la trajectoire de lancement prédite
        if (bShowLaunchTrajectory)
        {
            DrawLaunchTrajectory();
        }
        
        // Afficher si on est près de la singularité
        if (FMath::Abs(PendulumState.Angle) < 0.1f)
        {
            DrawDebugString(GetWorld(), CharPos + FVector(0, 0, 150), 
                          TEXT("ATTENTION: Proche singularité!"), 
                          nullptr, FColor::Red, 0.01f, true, 1.5f);
        }
    }
}

void USR_GrapplingHookComponent::DrawTrajectory()
{
    if (!bShowTrajectory || !bIsSwinging)
        return;
    
    // Simuler la trajectoire future
    float SimAngle = PendulumState.Angle;
    float SimAngularVelocity = PendulumState.AngularVelocity;
    FVector LastPos = OwnerCharacter->GetActorLocation();
    
    for (int i = 0; i < 50; i++)
    {
        // Simulation simplifiée du pendule simple
        float SimAngularAccel = -(Gravity / SwingCableLength) * FMath::Sin(SimAngle);
        
        SimAngularVelocity += SimAngularAccel * 0.1f;
        SimAngle += SimAngularVelocity * 0.1f;
        
        FVector SimPos = GrapplePoint + CalculateNewPosition(SimAngle, 0);
        
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

void USR_GrapplingHookComponent::OnMoveForwardInputPressed()
{
    bIsPressingForward = true;
}

void USR_GrapplingHookComponent::OnMoveForwardInputReleased()
{
    bIsPressingForward = false;
}

void USR_GrapplingHookComponent::OnMoveBackwardInputPressed()
{
    bIsPressingBackward = true;
}

void USR_GrapplingHookComponent::OnMoveBackwardInputReleased()
{
    bIsPressingBackward = false;
}

// Méthodes de l'interface ISR_State
void USR_GrapplingHookComponent::UpdateState() {}
bool USR_GrapplingHookComponent::LookAheadQuery() { return !bIsStateActive && bCanGrapple; }
FName USR_GrapplingHookComponent::GetStateName() const { return FName("GrapplingHook"); }
int32 USR_GrapplingHookComponent::GetStatePriority() const { return 1; }
bool USR_GrapplingHookComponent::IsStateActive() const { return bIsStateActive; }

void USR_GrapplingHookComponent::DrawLaunchTrajectory()
{
    if (!bShowLaunchTrajectory || !bIsSwinging)
        return;
    
    // Calculer la vélocité de lancement prédite
    FVector CurrentVelocity = PendulumState.Velocity;
    if (CurrentVelocity.Size() < 100.0f)
    {
        CurrentVelocity = OwnerCharacter->GetActorForwardVector() * 500.0f;
    }
    
    FVector LaunchDirection = CurrentVelocity.GetSafeNormal();
    float CurrentSpeed = CurrentVelocity.Size();
    FVector PredictedLaunchVelocity = LaunchDirection * (CurrentSpeed * LaunchBoostMultiplier);
    
    // Ajouter le boost vertical
    if (LaunchDirection.Z > 0.1f)
    {
        PredictedLaunchVelocity.Z += LaunchUpwardBoost * 1.5f;
    }
    else if (LaunchDirection.Z > -0.5f)
    {
        PredictedLaunchVelocity.Z += LaunchUpwardBoost;
    }
    else
    {
        PredictedLaunchVelocity.Z += LaunchUpwardBoost * 0.5f;
    }
    
    // Ajouter le boost d'amplitude si activé
    if (bUseAmplitudeBoost)
    {
        float AmplitudeBonus = FMath::Clamp(CurrentSwingAmplitude / MaxSwingAmplitude, 0.0f, 1.0f);
        PredictedLaunchVelocity *= (1.0f + AmplitudeBonus * MaxAmplitudeBoostPercent);
    }
    
    // Simuler la trajectoire balistique
    FVector CurrentPos = OwnerCharacter->GetActorLocation();
    FVector SimVelocity = PredictedLaunchVelocity;
    float TimeStep = 0.05f; // 50ms par step
    float MaxSimTime = 3.0f; // Simuler jusqu'à 3 secondes
    
    for (float SimTime = 0; SimTime < MaxSimTime; SimTime += TimeStep)
    {
        FVector NextPos = CurrentPos + SimVelocity * TimeStep;
        
        // Appliquer la gravité
        SimVelocity.Z -= Gravity * TimeStep;
        
        // Appliquer la résistance de l'air si activée
        if (bApplyAirResistanceToLaunch && SimVelocity.Size() > 0)
        {
            FVector AirResistance = -SimVelocity.GetSafeNormal() * SimVelocity.Size() * SimVelocity.Size() * LaunchAirResistance;
            SimVelocity += AirResistance * TimeStep;
        }
        
        // Dessiner le segment de trajectoire
        DrawDebugLine(GetWorld(), CurrentPos, NextPos, FColor::Orange, false, 0.01f, 0, 2.0f);
        
        // Vérifier si on touche le sol
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(OwnerCharacter);
        
        if (GetWorld()->LineTraceSingleByChannel(HitResult, CurrentPos, NextPos, ECC_Visibility, QueryParams))
        {
            // Marquer le point d'impact
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 30.0f, 12, FColor::Orange, false, 0.01f);
            
            // Afficher la distance et le temps de vol
            float Distance = FVector::Distance(OwnerCharacter->GetActorLocation(), HitResult.ImpactPoint);
            FString ImpactText = FString::Printf(TEXT("Distance: %.0f cm\nTemps: %.1fs"), Distance, SimTime);
            DrawDebugString(GetWorld(), HitResult.ImpactPoint + FVector(0, 0, 50), ImpactText, 
                           nullptr, FColor::Orange, 0.01f, true, 1.0f);
            break;
        }
        
        CurrentPos = NextPos;
    }
}